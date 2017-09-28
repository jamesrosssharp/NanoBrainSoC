--
--      NanoBrainSoC : system on chip built around the NanoBrain microcontroller
--      
--

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

use work.NanoBrainSoCMemory.all;
use work.NanoBrainCPU.all;

entity NanoBrainSoC is

  port (

    -- SRAM
    --SRAM_DQ   : inout std_logic_vector(15 downto 0);
    --SRAM_ADDR : out   std_logic_vector(17 downto 0);
    --SRAM_WE   : out   std_logic;
    --SRAM_CE   : out   std_logic;
    --SRAM_OE   : out   std_logic;
    --SRAM_LB   : out   std_logic;
    --SRAM_UB   : out   std_logic;

    -- SDRAM

    --DRAM_D    : inout std_logic_vector(15 downto 0);
    --DRAM_CLK  : out   std_logic;
    --DRAM_CKE  : out   std_logic;
    --DRAM_CS   : out   std_logic;
    --DRAM_WE   : out   std_logic;
    --DRAM_CAS  : out   std_logic;
    --DRAM_RAS  : out   std_logic;
    --DRAM_LDQM : out   std_logic;
    --DRAM_UDQM : out   std_logic;
    --DRAM_BA0  : out   std_logic;
    --DRAM_BA1  : out   std_logic;
    --DRAM_A    : out   std_logic(11 downto 0);

    -- Flash

    --FLASH_D     : inout std_logic_vector(7 downto 0);
    --FLASH_RESET : out   std_logic;
    --FLASH_WE    : out   std_logic;
    --FLASH_CE    : out   std_logic;
    --FLASH_OE    : out   std_logic;
    --FLASH_A     : out   std_logic_vector(21 downto 0);

    -- PS/2 Keyboard

    --PS2_DAT : inout std_logic;
    --PS2_CLK : out   std_logic;

    -- SD Card pins
    --SD_DAT  : in    std_logic;          -- MISO
    --SD_CMD  : out   std_logic;          -- MOSI
    --SD_CLK  : out   std_logic;          -- CLK
    --SD_DAT3 : inout std_logic;          -- \CS / card present

    -- UART Pins
    UART_TXD : out std_logic;
    UART_RXD : in  std_logic;

    -- clocks
    CLOCK_50 : in std_logic;
    CLOCK_24 : in std_logic;
    CLOCK_27 : in std_logic;

    -- keys 
    KEY : in std_logic_vector (3 downto 0);

    -- switches
    SW : in std_logic_vector(9 downto 0);

    -- LEDs

    LEDR : out std_logic_vector (9 downto 0);
    LEDG : out std_logic_vector (7 downto 0);

    -- Seven segment displays

    HEX0 : out std_logic_vector (6 downto 0);
    HEX1 : out std_logic_vector (6 downto 0);
    HEX2 : out std_logic_vector (6 downto 0);
    HEX3 : out std_logic_vector (6 downto 0)

    -- Audio codec

    --AUD_ADCDAT  : in  std_logic;
    --AUD_BCLK    : out std_logic;
    --AUD_DACDAT  : out std_logic;
    --AUD_ADCLRCK : out std_logic;
    --AUD_DACLRCK : out std_logic;

    --I2C_SDAT : inout std_logic;
    --I2C_SCLK : out   std_logic;
    --AUD_XCK  : out   std_logic;

    -- VGA DAC

    --VGA_R  : out std_logic (3 downto 0);
    --VGA_G  : out std_logic (3 downto 0);
    --VGA_B  : out std_logic (3 downto 0);
    --VGA_HS : out std_logic;
    --VGA_VS : out std_logic

    );

end NanoBrainSoC;

architecture RTL of NanoBrainSoC is
  signal clk   : std_logic;
  signal reset : std_logic;

  signal address_1    : unsigned(22 downto 0);
  signal wr_data_1    : std_logic_vector (15 downto 0);
  signal rd_data_1    : std_logic_vector (15 downto 0);
  signal burst_size_1 : std_logic_vector (3 downto 0);
  signal wr_req_1     : std_logic;
  signal rd_req_1     : std_logic;
  signal wr_grant_1   : std_logic;
  signal rd_grant_1   : std_logic;
  signal wr_done_1    : std_logic;
  signal n_rd_1       : std_logic;
  signal n_wr_1       : std_logic;
  signal address_2    : unsigned (22 downto 0);
  signal wr_data_2    : std_logic_vector (15 downto 0);
  signal rd_data_2    : std_logic_vector (15 downto 0);
  signal burst_size_2 : std_logic_vector (3 downto 0);
  signal wr_req_2     : std_logic;
  signal rd_req_2     : std_logic;
  signal wr_grant_2   : std_logic;
  signal rd_grant_2   : std_logic;
  signal wr_done_2    : std_logic;
  signal n_rd_2       : std_logic;
  signal n_wr_2       : std_logic;
  signal address_3    : unsigned (22 downto 0);
  signal wr_data_3    : std_logic_vector (15 downto 0);
  signal rd_data_3    : std_logic_vector (15 downto 0);
  signal burst_size_3 : std_logic_vector (3 downto 0);
  signal wr_req_3     : std_logic;
  signal rd_req_3     : std_logic;
  signal wr_grant_3   : std_logic;
  signal rd_grant_3   : std_logic;
  signal wr_done_3    : std_logic;
  signal n_rd_3       : std_logic;
  signal n_wr_3       : std_logic;
  signal address_4    : unsigned (22 downto 0);
  signal wr_data_4    : std_logic_vector (15 downto 0);
  signal rd_data_4    : std_logic_vector (15 downto 0);
  signal burst_size_4 : std_logic_vector (3 downto 0);
  signal wr_req_4     : std_logic;
  signal rd_req_4     : std_logic;
  signal wr_grant_4   : std_logic;
  signal rd_grant_4   : std_logic;
  signal wr_done_4    : std_logic;
  signal n_rd_4       : std_logic;
  signal n_wr_4       : std_logic;
  signal address_5    : unsigned (22 downto 0);
  signal wr_data_5    : std_logic_vector (15 downto 0);
  signal rd_data_5    : std_logic_vector (15 downto 0);
  signal burst_size_5 : std_logic_vector (3 downto 0);
  signal wr_req_5     : std_logic;
  signal rd_req_5     : std_logic;
  signal wr_grant_5   : std_logic;
  signal rd_grant_5   : std_logic;
  signal wr_done_5    : std_logic;
  signal n_rd_5       : std_logic;
  signal n_wr_5       : std_logic;
  signal address_6    : unsigned (22 downto 0);
  signal wr_data_6    : std_logic_vector (15 downto 0);
  signal rd_data_6    : std_logic_vector (15 downto 0);
  signal burst_size_6 : std_logic_vector (3 downto 0);
  signal wr_req_6     : std_logic;
  signal rd_req_6     : std_logic;
  signal wr_grant_6   : std_logic;
  signal rd_grant_6   : std_logic;
  signal wr_done_6    : std_logic;
  signal n_rd_6       : std_logic;
  signal n_wr_6       : std_logic;
  signal address_7    : unsigned (22 downto 0);
  signal wr_data_7    : std_logic_vector (15 downto 0);
  signal rd_data_7    : std_logic_vector (15 downto 0);
  signal burst_size_7 : std_logic_vector (3 downto 0);
  signal wr_req_7     : std_logic;
  signal rd_req_7     : std_logic;
  signal wr_grant_7   : std_logic;
  signal rd_grant_7   : std_logic;
  signal wr_done_7    : std_logic;
  signal n_rd_7       : std_logic;
  signal n_wr_7       : std_logic;
  signal address_8    : unsigned (22 downto 0);
  signal wr_data_8    : std_logic_vector (15 downto 0);
  signal rd_data_8    : std_logic_vector (15 downto 0);
  signal burst_size_8 : std_logic_vector (3 downto 0);
  signal wr_req_8     : std_logic;
  signal rd_req_8     : std_logic;
  signal wr_grant_8   : std_logic;
  signal rd_grant_8   : std_logic;
  signal wr_done_8    : std_logic;
  signal n_rd_8       : std_logic;
  signal n_wr_8       : std_logic;


  signal i_address    : unsigned (23 downto 0);
  signal i_wr_data    : std_logic_vector (15 downto 0);
  signal i_rd_data    : std_logic_vector (15 downto 0);
  signal i_burst_size : std_logic_vector (3 downto 0);
  signal i_wr_req     : std_logic;
  signal i_rd_req     : std_logic;
  signal i_wr_grant   : std_logic;
  signal i_rd_grant   : std_logic;
  signal i_wr_done    : std_logic;
  signal i_n_rd       : std_logic;
  signal i_n_wr       : std_logic;

  signal d_address    : unsigned (23 downto 0);
  signal d_wr_data    : std_logic_vector (15 downto 0);
  signal d_rd_data    : std_logic_vector (15 downto 0);
  signal d_burst_size : std_logic_vector (3 downto 0);
  signal d_wr_req     : std_logic;
  signal d_rd_req     : std_logic;
  signal d_wr_grant   : std_logic;
  signal d_rd_grant   : std_logic;
  signal d_wr_done    : std_logic;
  signal d_n_rd       : std_logic;
  signal d_n_wr       : std_logic;

  signal interrupt : std_logic := '0';

begin

  clk   <= CLOCK_50;
  reset <= not KEY(0);

  -- Nanobrain

  address_1    <= i_address (23 downto 1);
  wr_data_1    <= i_wr_data;
  i_rd_data    <= rd_data_1;
  burst_size_1 <= i_burst_size;
  wr_req_1     <= i_wr_req;
  rd_req_1     <= i_rd_req;
  i_wr_grant   <= wr_grant_1;
  i_rd_grant   <= rd_grant_1;
  i_wr_done    <= wr_done_1;
  n_rd_1       <= i_n_rd;
  n_wr_1       <= i_n_wr;

  address_2    <= d_address (23 downto 1);
  wr_data_2    <= d_wr_data;
  d_rd_data    <= rd_data_2;
  burst_size_2 <= d_burst_size;
  wr_req_2     <= d_wr_req;
  rd_req_2     <= d_rd_req;
  d_wr_grant   <= wr_grant_2;
  d_rd_grant   <= rd_grant_2;
  d_wr_done    <= wr_done_2;
  n_rd_2       <= d_n_rd;
  n_wr_2       <= d_n_wr;

  nb0 : NanoBrain
    generic map (
      DCACHE_NUM_LINES_BITS => 4,
      DCACHE_WIDTH_BITS     => 3,

      ICACHE_NUM_LINES_BITS => 9,
      ICACHE_WIDTH_BITS     => 3,

      USE_FPU => 0,
      USE_BS  => 0,
      USE_MUL => 0,
      USE_DIV => 0,
      USE_MMU => 0
      )
    port map (

      clk   => clk,
      reset => reset,

      i_address    => i_address,
      i_wr_data    => i_wr_data,
      i_rd_data    => i_rd_data,
      i_burst_size => i_burst_size,
      i_wr_req     => i_wr_req,
      i_rd_req     => i_rd_req,
      i_wr_grant   => i_wr_grant,
      i_rd_grant   => i_rd_grant,
      i_wr_done    => i_wr_done,
      i_n_rd       => i_n_rd,
      i_n_wr       => i_n_wr,

      d_address    => d_address,
      d_wr_data    => d_wr_data,
      d_rd_data    => d_rd_data,
      d_burst_size => d_burst_size,
      d_wr_req     => d_wr_req,
      d_rd_req     => d_rd_req,
      d_wr_grant   => d_wr_grant,
      d_rd_grant   => d_rd_grant,
      d_wr_done    => d_wr_done,
      d_n_rd       => d_n_rd,
      d_n_wr       => d_n_wr,

      interrupt => interrupt

      );

-- Memory controller

mem0 : NanoBrainSoCMemoryController
  generic map (
    ADDRESS_WIDTH    => 23,
    BR_ADDRESS_WIDTH => 4,
    WORD_WIDTH       => 16,
    BURST_SIZE_BITS  => 4
    )
port map (
  clk   => clk,
  reset => reset,

  address_1    => address_1,
  wr_data_1    => wr_data_1,
  rd_data_1    => rd_data_1,
  burst_size_1 => burst_size_1,
  wr_req_1     => wr_req_1,
  rd_req_1     => rd_req_1,
  wr_grant_1   => wr_grant_1,
  rd_grant_1   => rd_grant_1,
  wr_done_1    => wr_done_1,
  n_rd_1       => n_rd_1,
  n_wr_1       => n_wr_1,

  address_2    => address_2,
  wr_data_2    => wr_data_2,
  rd_data_2    => rd_data_2,
  burst_size_2 => burst_size_2,
  wr_req_2     => wr_req_2,
  rd_req_2     => rd_req_2,
  wr_grant_2   => wr_grant_2,
  rd_grant_2   => rd_grant_2,
  wr_done_2    => wr_done_2,
  n_rd_2       => n_rd_2,
  n_wr_2       => n_wr_2,

  address_3    => address_3,
  wr_data_3    => wr_data_3,
  rd_data_3    => rd_data_3,
  burst_size_3 => burst_size_3,
  wr_req_3     => wr_req_3,
  rd_req_3     => rd_req_3,
  wr_grant_3   => wr_grant_3,
  rd_grant_3   => rd_grant_3,
  wr_done_3    => wr_done_3,
  n_rd_3       => n_rd_3,
  n_wr_3       => n_wr_3,

  address_4    => address_4,
  wr_data_4    => wr_data_4,
  rd_data_4    => rd_data_4,
  burst_size_4 => burst_size_4,
  wr_req_4     => wr_req_4,
  rd_req_4     => rd_req_4,
  wr_grant_4   => wr_grant_4,
  rd_grant_4   => rd_grant_4,
  wr_done_4    => wr_done_4,
  n_rd_4       => n_rd_4,
  n_wr_4       => n_wr_4,

  address_5    => address_5,
  wr_data_5    => wr_data_5,
  rd_data_5    => rd_data_5,
  burst_size_5 => burst_size_5,
  wr_req_5     => wr_req_5,
  rd_req_5     => rd_req_5,
  wr_grant_5   => wr_grant_5,
  rd_grant_5   => rd_grant_5,
  wr_done_5    => wr_done_5,
  n_rd_5       => n_rd_5,
  n_wr_5       => n_wr_5,

  address_6    => address_6,
  wr_data_6    => wr_data_6,
  rd_data_6    => rd_data_6,
  burst_size_6 => burst_size_6,
  wr_req_6     => wr_req_6,
  rd_req_6     => rd_req_6,
  wr_grant_6   => wr_grant_6,
  rd_grant_6   => rd_grant_6,
  wr_done_6    => wr_done_6,
  n_rd_6       => n_rd_6,
  n_wr_6       => n_wr_6,

  address_7    => address_7,
  wr_data_7    => wr_data_7,
  rd_data_7    => rd_data_7,
  burst_size_7 => burst_size_7,
  wr_req_7     => wr_req_7,
  rd_req_7     => rd_req_7,
  wr_grant_7   => wr_grant_7,
  rd_grant_7   => rd_grant_7,
  wr_done_7    => wr_done_7,
  n_rd_7       => n_rd_7,
  n_wr_7       => n_wr_7,

  address_8    => address_8,
  wr_data_8    => wr_data_8,
  rd_data_8    => rd_data_8,
  burst_size_8 => burst_size_8,
  wr_req_8     => wr_req_8,
  rd_req_8     => rd_req_8,
  wr_grant_8   => wr_grant_8,
  rd_grant_8   => rd_grant_8,
  wr_done_8    => wr_done_8,
  n_rd_8       => n_rd_8,
  n_wr_8       => n_wr_8

  );

-- io port for LEDs

-- io port for hex displays

-- interrupt controller top level

-- uart top level

-- io port for switches

-- SD card top level

-- flash controller top level

-- audio core top level

-- graphics core top level

end RTL;
