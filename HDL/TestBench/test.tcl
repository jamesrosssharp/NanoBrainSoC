add wave /nanobrainsoc_tb/tb_clock_50
add wave /nanobrainsoc_tb/tb_key 
add wave /nanobrainsoc_tb/tb_uart_txd
add wave -radix ascii /nanobrainsoc_tb/tx_byte
add wave /nanobrainsoc_tb/tb_key 

add wave -radix hex /nanobrainsoc_tb/nb0/nb0/core0/pipe0/icache_rd_data_a 
add wave -radix hex /nanobrainsoc_tb/nb0/nb0/core0/pipe0/icache_address_a 
add wave -radix hex /nanobrainsoc_tb/nb0/nb0/core0/pipe0/icache_rd_data_b 
add wave -radix hex /nanobrainsoc_tb/nb0/nb0/core0/pipe0/icache_address_b 
add wave  /nanobrainsoc_tb/nb0/nb0/core0/pipe0/icache_rd_req_a  
add wave  /nanobrainsoc_tb/nb0/nb0/core0/pipe0/icache_rd_req_b

add wave  -radix hex /nanobrainsoc_tb/nb0/nb0/core0/pipe0/pc  
add wave  -radix hex /nanobrainsoc_tb/nb0/nb0/core0/pipe0/pc_next

add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/icache_rd_ready_a
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/icache_rd_data_a
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/icache_rd_ready_b
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/icache_rd_data_b
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/flush_pipeline
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_0_halt
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_0_icache_portsel
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/branch_target
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_1_halt
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_1_icache_portsel
add wave -radix hex /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_1_pc
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_2_stall
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_2_halt
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/decoded_ipu_op
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/decoded_bs_op
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/decoded_op
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/decoded_fc_op
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/decoded_io_op
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/decoded_operand_x
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/decoded_operand_y
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/decoded_operand_fx
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/decoded_operand_fy
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/decoded_reg16_dest_lo
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/decoded_reg16_dest_hi
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/decoded_jump_target
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/decoded_fpu_op
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/decoded_operand_z

add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_3_ipu_op
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_3_fpu_op
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_3_bs_op
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_3_op
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_3_fc_op
add wave /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_3_io_op

add wave -radix hex /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_3_ipu_operand_x
add wave -radix hex /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_3_ipu_operand_y
add wave -radix hex /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_3_ipu_operand_z
add wave -radix hex /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_3_fpu_operand_x
add wave -radix hex /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_3_fpu_operand_y
add wave -radix hex /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_3_reg16_dest_lo
add wave -radix hex /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_3_reg16_dest_hi
add wave -radix hex /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_3_jump_target

add wave -radix hex /nanobrainsoc_tb/nb0/nb0/core0/pipe0/imm_reg
add wave -radix hex /nanobrainsoc_tb/nb0/nb0/core0/pipe0/imm_reg_next

add wave -radix hex  /nanobrainsoc_tb/nb0/nb0/core0/pipe0/stage_2_instruction

add wave -radix hex /nanobrainsoc_tb/nb0/port_address
add wave -radix hex /nanobrainsoc_tb/nb0/port_rd_data
add wave -radix hex /nanobrainsoc_tb/nb0/port_wr_data
add wave /nanobrainsoc_tb/nb0/port_n_rd
add wave /nanobrainsoc_tb/nb0/port_n_wr

view structure .main_pane.structure.interior.cs.body.struct
view signals .main_pane.objects.interior.cs.body.tree
run 1 ms
