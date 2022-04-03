use std::{sync::{mpsc, atomic::{AtomicBool, Ordering}, Arc}, io::{BufReader, Read}, thread::JoinHandle};

use serial_rs::SerialPort;



pub struct SlipReader {
    pub queue: mpsc::Receiver<Vec<u8>>,
    stop_err: Option<String>,
    running: Arc<AtomicBool>,
    handle: Option<JoinHandle<()>>
}

impl SlipReader {
    pub fn new(mut port: Box<dyn SerialPort>) -> Self {
        let (sender, receiver) = mpsc::channel::<Vec<u8>>();

        let is_running = Arc::new(AtomicBool::new(true));
        let is_running_t = is_running.clone();

        let handle = std::thread::spawn(move|| {
            let mut partial_packet: Option<Vec<u8>> = None;
            let mut in_escape: bool = false;
            let mut successful_slip: bool = false;
            let mut buffer: Vec<u8> = Vec::new();
            println!("SLIP reader started");
            while is_running_t.load(Ordering::Relaxed) {
                let mut waiting = port.bytes_to_read().unwrap() as usize;
                if waiting != 0 {
                    println!("Port has {} bytes to read", waiting);
                    buffer = vec![0; waiting];
                    if let Err(e) = port.read_exact(&mut buffer) {
                        // Could not read the buffer!?
                        eprintln!("SLIP error reading buffer: {}", e);
                        return;
                    }
                    for b in &buffer {
                        let mut delete_packet = false;
                        match partial_packet.as_mut() {
                            None => {
                                if *b == 0xC0 {
                                    partial_packet = Some(vec![0xC0]);
                                } else {
                                    eprintln!("SLIP error reading invalid data. Buffer: {:02X?}", buffer);
                                    return;
                                }
                            }
                            Some(p_packet) => {
                                if in_escape {
                                    in_escape = false;
                                    if *b == 0xDC {
                                        p_packet.push(0xC0)
                                    } else if *b == 0xDD {
                                        p_packet.push(0xDB)
                                    } else {
                                        eprintln!("SLIP error reading invalid escape. Buffer: {:02X?}", buffer);
                                        return;
                                    }
                                } else if *b == 0xDB {
                                    in_escape = true
                                } else if *b == 0xC0 {
                                    sender.send(p_packet.clone());
                                    delete_packet = true;
                                    successful_slip = true;
                                } else {
                                    p_packet.push(*b);
                                }
                            }
                        }
                        if delete_packet {
                            partial_packet = None;
                        }
                    }
                } else {
                    std::thread::sleep(std::time::Duration::from_millis(5));
                }
            }
            println!("SLIP reader stop");
        });
        Self {
            queue: receiver,
            stop_err: None,
            handle: Some(handle),
            running: is_running
        }
    }
}

impl Drop for SlipReader {
    fn drop(&mut self) {
        self.running.store(false, Ordering::Relaxed);
        self.handle.take().unwrap().join();
    }
}