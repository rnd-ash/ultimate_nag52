import sys

file_name = sys.argv[1]
frame_name = sys.argv[2]

lines = open(file_name, 'r').readlines()


def clear_bit(mask, bit):
    return mask & ~(1<<bit)

def get_data_type(len: int) -> str:
    if len == 1:
        return "bool"
    elif len <= 8:
        return "uint8_t"
    elif len <= 16:
        return "short"
    elif len <= 32:
        return "int"
    else:
        raise "> 64bit numbers not handled!"


#print("{0:{fill}64b}".format(mask, fill = '0'))

def get_param_text(name: str, desc: str, offset: int, length: int) -> str:
    # assume all CAN Frames are 64bits in size, and use BigEndian (All MB ECUs in W203 do!)

    if 64-offset-length < 0:
        raise "Shift is less than 0!?"

    mask = 0xFFFFFFFFFFFFFFFF

    start_mask = 63-offset
    for bit in range(0,length):
        mask = clear_bit(mask, start_mask-bit) 

    f_mask = 0x0
    for bit in range(0,length):
        f_mask = (f_mask | 0x01 << bit)


    string =  "    // Sets {}\n".format(desc)
    string += "    void set_{}({} value){{ raw = (raw & 0x{:{fill}16x}) | ((uint64_t)value & 0x{:x}) << {}; }}\n".format(name, get_data_type(length), mask, f_mask, 64-length-offset, fill='0')
    string += "    // Gets {}\n".format(desc)
    string += "    {} get_{}() {{ return raw >> {} & 0x{:x}; }}\n".format(get_data_type(length), name, offset, f_mask)
    return string


entries = []
located_frame = False
frame_id = ""
for line in lines:
    if line.strip().startswith("FRAME"):
        if line.strip().startswith("FRAME {}".format(frame_name)):
            frame_id = line.split("(")[1].split(")")[0]
            located_frame = True
        elif located_frame: # next frame located, we are done loading lines!
            break
    elif located_frame and not line.strip().startswith("RAW:"):
        # ONLY ADD ENTRIES - NOT ENUM VALUES
        entries.append(line.strip())

if located_frame == False:
    raise Exception("Frame not found!")

res = ""
print("Found {} entries for {}".format(len(entries), frame_name))

res += "#include <stdint.h>\n"
res += "#include <can_common.h>\n"
res += "\n"
res += "#define {}_ID {}\n".format(frame_name.rstrip('h'), frame_id)
res += "\n"
res += "typedef union {\n"
res += "    uint8_t bytes[8];\n"
res += "    uint64_t raw;\n"

for entry in entries:
    name = entry.split(": ")[1].split(",")[0]
    offset = int(entry.split("OFFSET ")[1].split(" LEN")[0])
    length = int(entry.split("LEN ")[1].split(" ")[0])
    desc = entry.split(" - ")[1]

    res += get_param_text(name, desc, offset, length)
    res += "\n"

# Export frame function
res += "    void export_frame(CAN_FRAME &f) {\n"
res += "        f.id = {}_ID;\n".format(frame_name.rstrip('h'))
res += "        f.length = 8;\n"
res += "        f.priority = 4;\n"
res += "        f.rtr = false;\n"
res += "        f.extended = false;\n"
res += "        for (int i = 0; i < 7; i++) {\n"
res += "            f.data.bytes[i] = bytes[7-i];\n"
res += "        }\n"
res += "    }\n"

res += "}} {};\n".format(frame_name.rstrip('h'))

print(res)