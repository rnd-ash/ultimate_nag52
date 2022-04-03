use std::{path::PathBuf, fmt::Write, io::{Read, BufRead}, u8};

pub type ElfResult<T> = std::result::Result<T, ElfError>;

pub const LEN_FILE_HEADER: usize = 0x34;
pub const SEC_TYPE_PROG_BITS: u8 = 0x01;
pub const SEC_TYPE_STR_TAB: u8 = 0x03;
pub const SEC_TYPE_INIT_ARRAY: u8 = 0x0E;
pub const SEC_TYPE_FINI_ARRAY: u8 = 0x0F;

pub const PROG_SEC_TYPES: [u8; 3] = [SEC_TYPE_PROG_BITS, SEC_TYPE_INIT_ARRAY, SEC_TYPE_FINI_ARRAY];
pub const SEG_TYPE_LOAD: u8 = 0x01;
pub const LEN_SEG_HEADER: u64 = 0x20;
pub const LEN_SEC_HEADER: u64 = 0x28;


pub enum ElfError {
    InvalidMagicHeader,
    NonXtensaElfFile(u32),
    UnexpectedHeaderEntrySize(usize),
    NoHeader,
    InvalidReadSize(u64, u64),
    NoStrTab(u64),
    IoError(std::io::Error)
}

impl std::fmt::Debug for ElfError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::InvalidMagicHeader => write!(f, "InvalidMagicHeader"),
            Self::NonXtensaElfFile(eMachine) => write!(f, "NonXtensaElfFile(0x{:04X})", eMachine),
            Self::UnexpectedHeaderEntrySize(size) => write!(f, "UnexpectedHeaderEntrySize({})", size),
            Self::NoHeader => write!(f, "NoHeader"),
            Self::IoError(arg0) => f.debug_tuple("IoError").field(arg0).finish(),
            ElfError::InvalidReadSize(want, read) => write!(f, "InvalidReadSize({},{})", want, read),
            Self::NoStrTab(offset) => write!(f, "NoStrTab({})", offset),
        }
    }
}

impl std::error::Error for ElfError {
    fn source(&self) -> Option<&(dyn std::error::Error + 'static)> {
        match self {
            Self::IoError(e) => Some(e),
            _ => None
        }
    }

    fn cause(&self) -> Option<&dyn std::error::Error> {
        self.source()
    }
}

impl std::fmt::Display for ElfError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            ElfError::InvalidMagicHeader => write!(f, "Invalid EFL magic header"),
            ElfError::NonXtensaElfFile(eMachine) => write!(f, "ELF is not a Xtensa ELF file. E_MACHINE: 0x{:04X}", eMachine),
            ElfError::UnexpectedHeaderEntrySize(size) => write!(f, "ELF header is invalid size 0x{:04X} (not 0x{:04X})", size, LEN_SEC_HEADER),
            ElfError::NoHeader => write!(f, "ELF has 0 section headers"),
            ElfError::IoError(e) => write!(f, "IO Error {}", e),
            ElfError::InvalidReadSize(want, read) => write!(f, "Could not read {} bytes from ELF. Got {}", want, read),
            ElfError::NoStrTab(offset) => write!(f, "No STRTAB section at offset 0x{:04X}", offset),
        }
    }
}

impl From<std::io::Error> for ElfError {
    fn from(e: std::io::Error) -> Self {
        ElfError::IoError(e)
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub struct ElfSectionHeader {
    pub (self) name_offset: u32,
    pub (self) sec_type: u32,
    pub (self) flags: u32,
    pub (self) lma: u32,
    pub (self) sec_offsets: u32,
    pub (self) size: u32
}

impl ElfSectionHeader {
    pub (crate) fn from_bytes(b: &[u8]) -> Self {
        Self {
            name_offset: u32::from_le_bytes(b[0..4].try_into().unwrap()),
            sec_type: u32::from_le_bytes(b[4..8].try_into().unwrap()),
            flags: u32::from_le_bytes(b[8..12].try_into().unwrap()),
            lma: u32::from_le_bytes(b[12..16].try_into().unwrap()),
            sec_offsets: u32::from_le_bytes(b[16..20].try_into().unwrap()),
            size: u32::from_le_bytes(b[20..24].try_into().unwrap()),
        }
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub struct ElfSegmentHeader {
    pub (self) seg_type: u32,
    pub (self) seg_offset: u32,
    pub (self) va_addr: u32,
    pub (self) lma: u32,
    pub (self) size: u32,
    pub (self) mem_size: u32,
    pub (self) flags: u32,
    pub (self) align: u32
}

impl ElfSegmentHeader {
    pub (crate) fn from_bytes(b: &[u8]) -> Self {
        Self {
            seg_type: u32::from_le_bytes(b[0..4].try_into().unwrap()),
            seg_offset: u32::from_le_bytes(b[4..8].try_into().unwrap()),
            va_addr: u32::from_le_bytes(b[8..12].try_into().unwrap()),
            lma: u32::from_le_bytes(b[12..16].try_into().unwrap()),
            size: u32::from_le_bytes(b[16..20].try_into().unwrap()),
            mem_size: u32::from_le_bytes(b[20..24].try_into().unwrap()),
            flags: u32::from_le_bytes(b[24..28].try_into().unwrap()),
            align: u32::from_le_bytes(b[28..32].try_into().unwrap()),
        }
    }
}

#[derive(Debug, Clone, PartialEq, Eq, PartialOrd, Ord)]
pub struct ElfSection {
    pub name: String,
    pub addr: u64,
    pub data: Vec<u8>
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord)]
pub struct ElfHeader {
    pub (self) ident: [u8; 16],
    pub (self) elf_type: u16,
    pub (self) machine: u16,
    pub (self) version: u32,
    pub (self) entry_point: u32,
    pub (self) phoff: u32,
    pub (self) shoff: u32,
    pub (self) flags: u32,
    pub (self) ehsize: u16,
    pub (self) phentsize: u16,
    pub (self) phnum: u16,
    pub (self) shentsize: u16,
    pub (self) shnum: u16,
    pub (self) shstrndx: u16
}

impl ElfHeader {
    pub (crate) fn from_bytes(b: &[u8]) -> Self {
        Self {
            ident: b[0..16].try_into().unwrap(),
            elf_type: u16::from_le_bytes(b[16..18].try_into().unwrap()),
            machine: u16::from_le_bytes(b[18..20].try_into().unwrap()),
            version: u32::from_le_bytes(b[20..24].try_into().unwrap()),
            entry_point: u32::from_le_bytes(b[24..28].try_into().unwrap()),
            phoff: u32::from_le_bytes(b[28..32].try_into().unwrap()),
            shoff: u32::from_le_bytes(b[32..36].try_into().unwrap()),
            flags: u32::from_le_bytes(b[36..40].try_into().unwrap()),
            ehsize: u16::from_le_bytes(b[40..42].try_into().unwrap()),
            phentsize: u16::from_le_bytes(b[42..44].try_into().unwrap()),
            phnum: u16::from_le_bytes(b[44..46].try_into().unwrap()),
            shentsize: u16::from_le_bytes(b[46..48].try_into().unwrap()),
            shnum: u16::from_le_bytes(b[48..50].try_into().unwrap()),
            shstrndx: u16::from_le_bytes(b[50..52].try_into().unwrap()),
        }
    }
}

pub struct EspElfFile {
    header: ElfHeader,
    sections: Vec<ElfSection>,
    segments: Vec<ElfSection>
}


impl EspElfFile {
    pub fn new(path: &str) -> ElfResult<Self> { // esptool.py ElfFile._read_elf_file()
        let mut f = std::fs::File::open(path)?;
        let mut elf_bytes: Vec<u8> = Vec::new();
        let _ = f.read_to_end(&mut elf_bytes)?;
        let header = ElfHeader::from_bytes(&elf_bytes[0..LEN_FILE_HEADER]);
        println!("{:02X?}", header);
        if header.ident[0] != 0x7F || String::from_utf8_lossy(&header.ident[1..4]) != "ELF" {
            return Err(ElfError::InvalidMagicHeader)
        } else if header.machine != 0x5E && header.machine != 0xF3 {
            return Err(ElfError::NonXtensaElfFile(header.machine as u32))
        } else if header.shentsize as u64 != LEN_SEC_HEADER {
            return Err(ElfError::UnexpectedHeaderEntrySize(header.shentsize as usize))
        } else if header.shnum == 0 {
            return Err(ElfError::NoHeader)
        }
        let mut cursor = std::io::Cursor::new(elf_bytes);

        let sections = Self::read_sections(&mut cursor, header.shoff as u64, header.shnum as u64, header.shstrndx as u64)?;
        let segments = Self::read_segments(&mut cursor, header.phoff as u64, header.phnum as u64, header.shstrndx as u64)?;
        for sec in &sections {
            println!("Elf section. name {}, address {:04X}, size: {}", sec.name, sec.addr, sec.data.len());
        }

        for sec in &segments {
            println!("ELF segment. name {}, address {:04X}, size: {}", sec.name, sec.addr, sec.data.len());
        }

        Ok(Self {
            header,
            sections,
            segments,
        })
    }

    fn read_sections(cursor: &mut std::io::Cursor<Vec<u8>>, header_offset: u64, header_count: u64, shstrndx: u64) -> ElfResult<Vec<ElfSection>> {
        cursor.set_position(header_offset);
        let byte_count = LEN_SEC_HEADER * header_count;
        let mut section_header: Vec<u8> = vec![0; byte_count as usize];
        if cursor.read(&mut section_header)? != byte_count as usize {
            return Err(ElfError::InvalidReadSize(byte_count, section_header.len() as u64))
        }

        let section_header_offsets: Vec<u64> = (0..section_header.len() as u64).step_by(LEN_SEC_HEADER as usize).collect();

        fn read_section_header(glob_header: &[u8], offset: u64) -> ElfSectionHeader {
            ElfSectionHeader::from_bytes(&glob_header[offset as usize..])
        }

        let all_sections: Vec<ElfSectionHeader> = section_header_offsets.iter().map(|offset|  read_section_header(&section_header, *offset) ).collect();
        let prog_sections: Vec<ElfSectionHeader> = all_sections.iter().filter(|s| PROG_SEC_TYPES.contains(&(s.sec_type as u8)) ).cloned().collect();

        if !section_header_offsets.contains(&(shstrndx * LEN_SEC_HEADER)) {
            return Err(ElfError::NoStrTab(shstrndx * LEN_SEC_HEADER))
        }

        let str_tab_header = read_section_header(&section_header, shstrndx * LEN_SEC_HEADER);

        cursor.set_position(str_tab_header.sec_offsets as u64);
        let mut string_table: Vec<u8> = vec![0; str_tab_header.size as usize];
        let _ = cursor.read_exact(&mut string_table)?;


        fn lookup_string(str_tbl: &[u8], offset: u64) -> String {
            let raw = &str_tbl[offset as usize..];
            let end = raw.iter().position(|x| *x == 0x00).unwrap();
            String::from_utf8(raw[..end].to_vec()).unwrap()
        }

        fn read_data(c: &mut std::io::Cursor<Vec<u8>>, offset: u64, size: usize) -> Vec<u8> {
            let mut buf: Vec<u8> = vec![0; size];
            c.set_position(offset);
            c.read_exact(&mut buf).unwrap();
            buf
        }

        let result_sections: Vec<ElfSection> = prog_sections.iter()
        .filter(|sec| sec.lma != 0 && sec.size > 0)
        .map(|sec| {
            ElfSection {
                name: lookup_string(&string_table, sec.name_offset as u64),
                addr: sec.lma as u64,
                data: read_data(cursor, sec.sec_offsets as u64, sec.size as usize),
            }
        }).collect();

        Ok(result_sections)
    }

    fn read_segments(cursor: &mut std::io::Cursor<Vec<u8>>, seg_header_offset: u64, seg_header_count: u64, shstrndx: u64) -> ElfResult<Vec<ElfSection>> {
        cursor.set_position(seg_header_offset);
        let byte_count = LEN_SEG_HEADER * seg_header_count;
        let mut segment_header: Vec<u8> = vec![0; byte_count as usize];
        if cursor.read(&mut segment_header)? != byte_count as usize {
            return Err(ElfError::InvalidReadSize(byte_count, segment_header.len() as u64))
        }

        let segment_header_offsets: Vec<u64> = (0..segment_header.len() as u64).step_by(LEN_SEG_HEADER as usize).collect();

        fn read_section_header(glob_header: &[u8], offset: u64) -> ElfSegmentHeader {
            ElfSegmentHeader::from_bytes(&glob_header[offset as usize..])
        }

        let all_segments: Vec<ElfSegmentHeader> = segment_header_offsets.iter().map(|offset|  read_section_header(&segment_header, *offset) ).collect();
        let prog_segments: Vec<ElfSegmentHeader> = all_segments.iter().filter(|s| s.seg_type == SEG_TYPE_LOAD as u32).cloned().collect();

        fn read_data(c: &mut std::io::Cursor<Vec<u8>>, offset: u64, size: usize) -> Vec<u8> {
            let mut buf: Vec<u8> = vec![0; size];
            c.set_position(offset);
            c.read_exact(&mut buf).unwrap();
            buf
        }

        let result_sections: Vec<ElfSection> = prog_segments.iter()
        .filter(|seg| seg.lma != 0 && seg.size > 0)
        .map(|seg| {
            ElfSection {
                name: "PHDR".to_string(),
                addr: seg.lma as u64,
                data: read_data(cursor, seg.seg_offset as u64, seg.size as usize),
            }
        }).collect();

        Ok(result_sections)
    }
}