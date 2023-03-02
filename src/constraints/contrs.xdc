set_property PACKAGE_PIN F6 [get_ports {gpio_rtl_tri_i[0]}]
set_property PACKAGE_PIN G5 [get_ports {gpio_rtl_tri_i[1]}]
set_property PACKAGE_PIN A6 [get_ports {gpio_rtl_tri_i[2]}]
set_property PACKAGE_PIN A7 [get_ports {gpio_rtl_tri_i[3]}]

set_property IOSTANDARD LVCMOS33 [get_ports {gpio_rtl_tri_i[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gpio_rtl_tri_i[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gpio_rtl_tri_i[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {gpio_rtl_tri_i[0]}]

create_pblock pblock_rp_const
add_cells_to_pblock [get_pblocks pblock_rp_const] [get_cells -quiet [list design_1_i/rp_const]]
resize_pblock [get_pblocks pblock_rp_const] -add {SLICE_X29Y120:SLICE_X36Y129}
resize_pblock [get_pblocks pblock_rp_const] -add {RAMB18_X3Y48:RAMB18_X4Y51}
resize_pblock [get_pblocks pblock_rp_const] -add {RAMB36_X3Y24:RAMB36_X4Y25}
set_property SNAPPING_MODE ON [get_pblocks pblock_rp_const]
