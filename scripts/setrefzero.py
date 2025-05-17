import re

file = 'anims/ref.smd'

print(file)

with open(file, 'r') as f:
    content = f.read()
    
mskeleton = re.search("skeleton", content)
anim = content[mskeleton.end():-3].split("time ")[1:]
hdr = content[:mskeleton.end()]

anims = []
for bone in anim:
    anims.append(bone.split("\n")[1:-1])

for bone in range(len(anims[0])):
    if bone not in [8, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113]: # custom/weapon bones
        rawv = anims[0][bone].split()[1:]
        calcrot = ' '.join("0.000000" for i in range(3))
        anims[0][bone] = f"{bone}  {' '.join(rawv[:3])}  {calcrot}"

with open('anims/ref.smd', 'w') as out:
    out.write(hdr)
    out.write("\ntime 0\n")
    out.write("\n".join(j for j in anims[0]))
    out.write("\nend\n")
