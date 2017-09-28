--
--      Test bench for NanoBrainSoC
--

library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity NanoBrainSoC_tb is
end NanoBrainSoC_tb;

architecture RTL of NanoBrainSoC_tb
is

  component NanoBrainSoC is

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

  end component;

  constant clk_period  : time := 0.020 us;
  constant bit_period  : time := 8680.5 ns;
  constant stop_period : time := 8680.5 ns;

  signal chars_transmitted : integer := 0;
  signal tx_byte           : std_logic_vector(7 downto 0);

  signal tb_uart_txd : std_logic;
  signal tb_uart_rxd : std_logic;

  signal tb_clock_50 : std_logic;
  signal tb_clock_24 : std_logic;
  signal tb_clock_27 : std_logic;

  signal tb_key : std_logic_vector (3 downto 0);

  signal tb_sw : std_logic_vector(9 downto 0);

  signal tb_ledr : std_logic_vector (9 downto 0);
  signal tb_ledg : std_logic_vector (7 downto 0);

  signal tb_hex0 : std_logic_vector (6 downto 0);
  signal tb_hex1 : std_logic_vector (6 downto 0);
  signal tb_hex2 : std_logic_vector (6 downto 0);
  signal tb_hex3 : std_logic_vector (6 downto 0);

begin

  nb0 : NanoBrainSoC port map (
    UART_TXD => tb_uart_txd,
    UART_RXD => tb_uart_rxd,
    CLOCK_50 => tb_clock_50,
    CLOCK_24 => tb_clock_24,
    CLOCK_27 => tb_clock_27,
    KEY      => tb_key,
    SW       => tb_sw,
    LEDR     => tb_ledr,
    LEDG     => tb_ledg,
    HEX0     => tb_hex0,
    HEX1     => tb_hex1,
    HEX2     => tb_hex2,
    HEX3     => tb_hex3
    );

  process
  begin
    tb_clock_50 <= '0';
    wait for clk_period / 2;
    tb_clock_50 <= '1';
    wait for clk_period / 2;
  end process;

  process
  begin
    wait for 10 ns;
    tb_key(0) <= '0';
    wait for 40 ns;
    tb_key(0) <= '1';
    wait for 10 ms;
  end process;

  -- process TX data
  process
    variable tx_byte_var : std_logic_vector (7 downto 0);
  begin

    wait until tb_uart_txd = '0';
    tx_byte_var := (others => '0');

    for i in tx_byte'reverse_range loop
      wait for bit_period;
      tx_byte_var(i) := tb_uart_txd;
    end loop;

    -- stop bit

    wait for stop_period;

    tx_byte <= tx_byte_var;

    chars_transmitted <= chars_transmitted + 1;

  end process;


end RTL;
