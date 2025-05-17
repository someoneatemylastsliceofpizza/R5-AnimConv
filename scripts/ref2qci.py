import re

with open('anims/ref.smd', 'r') as f:
    ref_content = f.read()

mnodes = re.search('nodes', ref_content)
mend = re.search('end', ref_content)
mtime = re.search('time 0', ref_content)

ref = ref_content[mnodes.end()+1:mend.start()-1].split("\n")
ref_time_0 = ref_content[mtime.end()+1:-5].split("\n")

ref_bone = []
for i in ref:
    ref_bone.append(i.split(" ")[1])
ref_bone.append("\"\"")
    
ref_parent_bone = []
for i in ref:
    ref_parent_bone.append(ref_bone[int(i.split(" ")[2])])

with open("ref.qci", "w") as output_file:
    output_file.write("$unlockdefinebones\n")
    for i in range(len(ref)):
        output_file.write(f"$definebone {ref_bone[i]} {ref_parent_bone[i]} {' '.join(ref_time_0[i].split(' ')[1:])} 0 0 0 0 0 0\n")
    for i in range(len(ref)):
        output_file.write(f"$bonemerge {ref_bone[i]}\n")