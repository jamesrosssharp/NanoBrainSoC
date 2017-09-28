--
--      NanoBrainSoCMemoryController : memory controller for NanoBrain SoC
--      
--

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

use work.MemoryControllers.all;
use work.NanoBrainSoCMemory.all;

entity NanoBrainSoCMemoryController is
  generic (
    ADDRESS_WIDTH    : natural := 23;
    BR_ADDRESS_WIDTH : natural := 8;
    WORD_WIDTH       : natural := 16;
    BURST_SIZE_BITS  : natural := 4
    );
  port (

    clk   : in std_logic;
    reset : in std_logic;

    address_1    : in  unsigned (ADDRESS_WIDTH - 1 downto 0);
    wr_data_1    : in  std_logic_vector (WORD_WIDTH - 1 downto 0);
    rd_data_1    : out std_logic_vector (WORD_WIDTH - 1 downto 0);
    burst_size_1 : in  std_logic_vector(BURST_SIZE_BITS - 1 downto 0);
    wr_req_1     : in  std_logic;
    rd_req_1     : in  std_logic;
    wr_grant_1   : out std_logic;
    rd_grant_1   : out std_logic;
    wr_done_1    : out std_logic;
    n_rd_1       : in  std_logic;
    n_wr_1       : in  std_logic;

    address_2    : in  unsigned (ADDRESS_WIDTH - 1 downto 0);
    wr_data_2    : in  std_logic_vector (WORD_WIDTH - 1 downto 0);
    rd_data_2    : out std_logic_vector (WORD_WIDTH - 1 downto 0);
    burst_size_2 : in  std_logic_vector(BURST_SIZE_BITS - 1 downto 0);
    wr_req_2     : in  std_logic;
    rd_req_2     : in  std_logic;
    wr_grant_2   : out std_logic;
    rd_grant_2   : out std_logic;
    wr_done_2    : out std_logic;
    n_rd_2       : in  std_logic;
    n_wr_2       : in  std_logic;

    address_3    : in  unsigned (ADDRESS_WIDTH - 1 downto 0);
    wr_data_3    : in  std_logic_vector (WORD_WIDTH - 1 downto 0);
    rd_data_3    : out std_logic_vector (WORD_WIDTH - 1 downto 0);
    burst_size_3 : in  std_logic_vector(BURST_SIZE_BITS - 1 downto 0);
    wr_req_3     : in  std_logic;
    rd_req_3     : in  std_logic;
    wr_grant_3   : out std_logic;
    rd_grant_3   : out std_logic;
    wr_done_3    : out std_logic;
    n_rd_3       : in  std_logic;
    n_wr_3       : in  std_logic;

    address_4    : in  unsigned (ADDRESS_WIDTH - 1 downto 0);
    wr_data_4    : in  std_logic_vector (WORD_WIDTH - 1 downto 0);
    rd_data_4    : out std_logic_vector (WORD_WIDTH - 1 downto 0);
    burst_size_4 : in  std_logic_vector(BURST_SIZE_BITS - 1 downto 0);
    wr_req_4     : in  std_logic;
    rd_req_4     : in  std_logic;
    wr_grant_4   : out std_logic;
    rd_grant_4   : out std_logic;
    wr_done_4    : out std_logic;
    n_rd_4       : in  std_logic;
    n_wr_4       : in  std_logic;

    address_5    : in  unsigned (ADDRESS_WIDTH - 1 downto 0);
    wr_data_5    : in  std_logic_vector (WORD_WIDTH - 1 downto 0);
    rd_data_5    : out std_logic_vector (WORD_WIDTH - 1 downto 0);
    burst_size_5 : in  std_logic_vector(BURST_SIZE_BITS - 1 downto 0);
    wr_req_5     : in  std_logic;
    rd_req_5     : in  std_logic;
    wr_grant_5   : out std_logic;
    rd_grant_5   : out std_logic;
    wr_done_5    : out std_logic;
    n_rd_5       : in  std_logic;
    n_wr_5       : in  std_logic;

    address_6    : in  unsigned (ADDRESS_WIDTH - 1 downto 0);
    wr_data_6    : in  std_logic_vector (WORD_WIDTH - 1 downto 0);
    rd_data_6    : out std_logic_vector (WORD_WIDTH - 1 downto 0);
    burst_size_6 : in  std_logic_vector(BURST_SIZE_BITS - 1 downto 0);
    wr_req_6     : in  std_logic;
    rd_req_6     : in  std_logic;
    wr_grant_6   : out std_logic;
    rd_grant_6   : out std_logic;
    wr_done_6    : out std_logic;
    n_rd_6       : in  std_logic;
    n_wr_6       : in  std_logic;

    address_7    : in  unsigned (ADDRESS_WIDTH - 1 downto 0);
    wr_data_7    : in  std_logic_vector (WORD_WIDTH - 1 downto 0);
    rd_data_7    : out std_logic_vector (WORD_WIDTH - 1 downto 0);
    burst_size_7 : in  std_logic_vector(BURST_SIZE_BITS - 1 downto 0);
    wr_req_7     : in  std_logic;
    rd_req_7     : in  std_logic;
    wr_grant_7   : out std_logic;
    rd_grant_7   : out std_logic;
    wr_done_7    : out std_logic;
    n_rd_7       : in  std_logic;
    n_wr_7       : in  std_logic;

    address_8    : in  unsigned (ADDRESS_WIDTH - 1 downto 0);
    wr_data_8    : in  std_logic_vector (WORD_WIDTH - 1 downto 0);
    rd_data_8    : out std_logic_vector (WORD_WIDTH - 1 downto 0);
    burst_size_8 : in  std_logic_vector(BURST_SIZE_BITS - 1 downto 0);
    wr_req_8     : in  std_logic;
    rd_req_8     : in  std_logic;
    wr_grant_8   : out std_logic;
    rd_grant_8   : out std_logic;
    wr_done_8    : out std_logic;
    n_rd_8       : in  std_logic;
    n_wr_8       : in  std_logic
    );
end NanoBrainSoCMemoryController;

architecture RTL of NanoBrainSoCMemoryController is

  signal sdram_address    : unsigned (ADDRESS_WIDTH - 2 downto 0);
  signal sdram_wr_data    : std_logic_vector (WORD_WIDTH - 1 downto 0);
  signal sdram_rd_data    : std_logic_vector (WORD_WIDTH - 1 downto 0);
  signal sdram_burst_size : std_logic_vector(BURST_SIZE_BITS - 1 downto 0);
  signal sdram_wr_req     : std_logic;
  signal sdram_rd_req     : std_logic;
  signal sdram_wr_grant   : std_logic;
  signal sdram_rd_grant   : std_logic;
  signal sdram_wr_done    : std_logic;
  signal sdram_n_rd       : std_logic;
  signal sdram_n_wr       : std_logic;

  signal flash_address    : unsigned (ADDRESS_WIDTH - 3 downto 0);
  signal flash_wr_data    : std_logic_vector (WORD_WIDTH - 1 downto 0);
  signal flash_rd_data    : std_logic_vector (WORD_WIDTH - 1 downto 0);
  signal flash_burst_size : std_logic_vector(BURST_SIZE_BITS - 1 downto 0);
  signal flash_wr_req     : std_logic;
  signal flash_rd_req     : std_logic;
  signal flash_wr_grant   : std_logic;
  signal flash_rd_grant   : std_logic;
  signal flash_wr_done    : std_logic;
  signal flash_n_rd       : std_logic;
  signal flash_n_wr       : std_logic;

  signal brc_address    : unsigned (ADDRESS_WIDTH - 3 downto 0);
  signal brc_wr_data    : std_logic_vector (WORD_WIDTH - 1 downto 0);
  signal brc_rd_data    : std_logic_vector (WORD_WIDTH - 1 downto 0);
  signal brc_burst_size : std_logic_vector(BURST_SIZE_BITS - 1 downto 0);
  signal brc_wr_req     : std_logic;
  signal brc_rd_req     : std_logic;
  signal brc_wr_grant   : std_logic;
  signal brc_rd_grant   : std_logic;
  signal brc_wr_done    : std_logic;
  signal brc_n_rd       : std_logic;
  signal brc_n_wr       : std_logic;

  signal br_address : natural range 0 to 2**BR_ADDRESS_WIDTH - 1;
  signal br_wr_data : std_logic_vector (WORD_WIDTH - 1 downto 0);
  signal br_wr      : std_logic;
  signal br_rd_data : std_logic_vector (WORD_WIDTH - 1 downto 0);

begin

  eightPort0 : EightPortMemoryController
    generic map (
      ADDRESS_WIDTH   => ADDRESS_WIDTH,
      WORD_WIDTH      => WORD_WIDTH,
      BURST_SIZE_BITS => BURST_SIZE_BITS
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
      n_wr_8       => n_wr_8,

      ds_address_1    => sdram_address,
      ds_wr_data_1    => sdram_wr_data,
      ds_rd_data_1    => sdram_rd_data,
      ds_burst_size_1 => sdram_burst_size,
      ds_wr_req_1     => sdram_wr_req,
      ds_rd_req_1     => sdram_rd_req,
      ds_wr_grant_1   => sdram_wr_grant,
      ds_rd_grant_1   => sdram_rd_grant,
      ds_wr_done_1    => sdram_wr_done,
      ds_n_rd_1       => sdram_n_rd,
      ds_n_wr_1       => sdram_n_wr,

      ds_address_2    => flash_address,
      ds_wr_data_2    => flash_wr_data,
      ds_rd_data_2    => flash_rd_data,
      ds_burst_size_2 => flash_burst_size,
      ds_wr_req_2     => flash_wr_req,
      ds_rd_req_2     => flash_rd_req,
      ds_wr_grant_2   => flash_wr_grant,
      ds_rd_grant_2   => flash_rd_grant,
      ds_wr_done_2    => flash_wr_done,
      ds_n_rd_2       => flash_n_rd,
      ds_n_wr_2       => flash_n_wr,

      ds_address_3    => brc_address,
      ds_wr_data_3    => brc_wr_data,
      ds_rd_data_3    => brc_rd_data,
      ds_burst_size_3 => brc_burst_size,
      ds_wr_req_3     => brc_wr_req,
      ds_rd_req_3     => brc_rd_req,
      ds_wr_grant_3   => brc_wr_grant,
      ds_rd_grant_3   => brc_rd_grant,
      ds_wr_done_3    => brc_wr_done,
      ds_n_rd_3       => brc_n_rd,
      ds_n_wr_3       => brc_n_wr
      );

  brc0 : BlockRamMemoryController
    generic map (
      ADDRESS_WIDTH    => ADDRESS_WIDTH - 2,
      BR_ADDRESS_WIDTH => BR_ADDRESS_WIDTH,
      WORD_WIDTH       => WORD_WIDTH,
      BURST_SIZE_BITS  => BURST_SIZE_BITS
      )
    port map (
      clk   => clk,
      reset => reset,

      address    => brc_address,
      wr_data    => brc_wr_data,
      rd_data    => brc_rd_data,
      burst_size => brc_burst_size,
      wr_req     => brc_wr_req,
      rd_req     => brc_rd_req,
      wr_grant   => brc_wr_grant,
      rd_grant   => brc_rd_grant,
      wr_done    => brc_wr_done,
      n_rd       => brc_n_rd,
      n_wr       => brc_n_wr,

      br_address => br_address,
      br_wr_data => br_wr_data,
      br_rd_data => br_rd_data,
      br_wr      => br_wr

      );

  br0 : NanoBrainSoCMemoryTemplate
    port map (
      clk  => clk,
      addr => br_address,
      data => br_wr_data,
      we   => br_wr,
      q    => br_rd_data
      );

end RTL;
