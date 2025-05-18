import re

with open('anims/ref.smd', 'r') as f:
    ref_content = f.read()
    
def read_target_order(path):
    with open(path, 'r') as f:
        return [line.strip() for line in f if line.strip()]

def parse_nodes_section(lines):
    nodes = []
    for line in lines:
        parts = line.strip().split()
        if len(parts) >= 3:
            bone_id = int(parts[0])
            bone_name = parts[1].strip('"')
            parent_id = int(parts[2])
            nodes.append((bone_id, bone_name, parent_id))
    return nodes

def process_smd(input_smd):
    with open(input_smd, 'r') as f:
        lines = f.readlines()

    in_nodes = False
    in_skeleton = False
    nodes_raw = []
    skeleton_raw = []

    for line in lines:
        stripped = line.strip()
        if stripped == 'nodes':
            in_nodes = True
        elif stripped == 'end' and in_nodes:
            in_nodes = False
        elif in_nodes:
            nodes_raw.append(stripped)

        elif stripped == 'skeleton':
            in_skeleton = True
        elif stripped == 'end' and in_skeleton:
            in_skeleton = False
        elif in_skeleton:
            skeleton_raw.append(stripped)
     
    return (nodes_raw, skeleton_raw)
    

rawref = process_smd('anims/ref.smd')


ref = rawref[0]
ref_time_0 = rawref[1][1:]

ref_bone = []
for i in ref:
    ref_bone.append(i.split(" ")[1])
ref_bone.append("\"\"")


ref_parent_bone = []
for i in ref:
    ref_parent_bone.append(ref_bone[int(i.split()[2])])

with open("ref.qci", "w") as output_file:
    output_file.write("$unlockdefinebones\n")
    for i in range(len(ref)):
        output_file.write(f"$definebone {ref_bone[i]} {ref_parent_bone[i]} {' '.join(ref_time_0[i].split(' ')[1:])} 0 0 0 0 0 0\n")
    for i in range(len(ref)):
        output_file.write(f"$bonemerge {ref_bone[i]}\n")