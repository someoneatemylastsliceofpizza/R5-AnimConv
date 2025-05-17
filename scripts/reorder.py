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

def reorder_nodes(nodes, target_order):
    name_to_node = {name: (id, name, parent) for id, name, parent in nodes}
    reordered = []
    used_names = set()

    for name in target_order:
        if name in name_to_node:
            reordered.append(name_to_node[name])
            used_names.add(name)

    for node in nodes:
        if node[1] not in used_names:
            reordered.append(node)

    name_to_new_id = {name: idx for idx, (_, name, _) in enumerate(reordered)}
    old_id_to_new = {}
    old_id_to_name = {id: name for id, name, _ in nodes}

    fixed_nodes = []
    for idx, (_, name, parent) in enumerate(reordered):
        old_id = [id for id, n, _ in nodes if n == name][0]
        old_id_to_new[old_id] = idx
        new_parent = name_to_new_id.get(nodes[parent][1], -1) if parent >= 0 else -1
        fixed_nodes.append(f'{idx} "{name}" {new_parent}\n')

    return fixed_nodes, old_id_to_new

def reorder_and_rewrite_skeleton(lines, id_map):
    output = []
    time_block = []
    in_time = False

    for line in lines:
        stripped = line.strip()
        if stripped.startswith('time '):
            if time_block:
                reordered = sorted(time_block, key=lambda l: id_map.get(int(l.split()[0]), 9999))
                output.extend(rewrite_bone_ids(reordered, id_map))
                time_block = []
            output.append(line)
            in_time = True
        elif in_time and stripped:
            time_block.append(line)
        else:
            if time_block:
                reordered = sorted(time_block, key=lambda l: id_map.get(int(l.split()[0]), 9999))
                output.extend(rewrite_bone_ids(reordered, id_map))
                time_block = []
            output.append(line)
            in_time = False

    if time_block:
        reordered = sorted(time_block, key=lambda l: id_map.get(int(l.split()[0]), 9999))
        output.extend(rewrite_bone_ids(reordered, id_map))

    return output

def rewrite_bone_ids(lines, id_map):
    rewritten = []
    for line in lines:
        parts = line.strip().split()
        if not parts:
            continue
        old_id = int(parts[0])
        new_id = id_map.get(old_id, old_id)
        rewritten.append(' '.join([str(new_id)] + parts[1:]) + '\n')
    return rewritten

def process_smd(input_smd, target_txt):
    with open(input_smd, 'r') as f:
        lines = f.readlines()

    target_order = read_target_order(target_txt)
    output_lines = []

    in_nodes = False
    in_skeleton = False
    nodes_raw = []
    skeleton_raw = []

    for line in lines:
        stripped = line.strip()
        if stripped == 'nodes':
            in_nodes = True
            output_lines.append(line)
        elif stripped == 'end' and in_nodes:
            in_nodes = False
            parsed_nodes = parse_nodes_section(nodes_raw)
            new_nodes, id_map = reorder_nodes(parsed_nodes, target_order)
            output_lines.extend(new_nodes)
            output_lines.append('end\n')
        elif in_nodes:
            nodes_raw.append(line)

        elif stripped == 'skeleton':
            in_skeleton = True
            output_lines.append(line)
        elif stripped == 'end' and in_skeleton:
            in_skeleton = False
            reordered_skeleton = reorder_and_rewrite_skeleton(skeleton_raw, id_map)
            output_lines.extend(reordered_skeleton)
            output_lines.append('end\n')
        elif in_skeleton:
            skeleton_raw.append(line)
        elif not in_nodes and not in_skeleton:
            output_lines.append(line)

    with open(input_smd, 'w') as f:
        f.writelines(output_lines)

# 100% chat gpt
process_smd('anims/draw.smd', 'target_ref.txt')
