a = input().split("\n")
b = [c.split(",")[1] for c in a]
print('\n'.join(b))


# bones[0],jx_c_delta,224h,B8h,struct mstudiobone_t_v54,Fg: Bg:0x407F40,
# bones[1],jx_c_start,2DCh,B8h,struct mstudiobone_t_v54,Fg: Bg:0x407F40,
# bones[2],jx_c_pov,394h,B8h,struct mstudiobone_t_v54,Fg: Bg:0x407F40,
# bones[3],jx_c_camera,44Ch,B8h,struct mstudiobone_t_v54,Fg: Bg:0x407F40,
# bones[4],def_c_hip,504h,B8h,struct mstudiobone_t_v54,Fg: Bg:0x407F40,
# bones[5],def_c_spineA,5BCh,B8h,struct mstudiobone_t_v54,Fg: Bg:0x407F40,
# bones[6],def_c_spineB,674h,B8h,struct mstudiobone_t_v54,Fg: Bg:0x407F40,