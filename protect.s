.extern see_start
.extern see_data
.extern see_data_end

.macro protect_range end, reg_end, reg_cfg, shift, pmp
  la t0, \end
  slli t0, t0, 8
  srli t0, t0, 10
  csrw \reg_end, t0

  csrr t0, \reg_cfg

  li a0, \pmp # 10001101 lock, TOR format, Read and Execute only
  slli a0, a0, \shift # write to pmp1cfg
  or a0, t0, a0

  csrrw a0, \reg_cfg, a0
.endm

.global protect_see
protect_see:
  # first protect the text segment
  la t0, see_start
  slli t0, t0, 8
  srli t0, t0, 10
  csrw pmpaddr0, t0

  protect_range see_data, pmpaddr1, pmpcfg0, 8, 0x8d
  protect_range see_data_end, pmpaddr2, pmpcfg0, 16, 0x8b
  
  ret

