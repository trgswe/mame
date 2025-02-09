// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria
/***************************************************************************

Hexion (GX122) (c) 1992 Konami

driver by Nicola Salmoria

Notes:
- The board has a 052591, which is used for protection in Thunder Cross and
  S.P.Y. In this game, however, the only thing it seems to do is clear the
  screen.
  This is the program for the 052591:
00: 5f 80 01 e0 08
01: df 80 00 e0 0c
02: df 90 02 e0 0c
03: df a0 03 e0 0c
04: df b0 0f e0 0c
05: df c0 ff bf 0c
06: 5c 02 00 33 0c
07: 5f 80 04 80 0c
08: 5c 0e 00 2b 0c
09: df 70 00 cb 08
0a: 5f 80 00 80 0c
0b: 5c 04 00 2b 0c
0c: df 60 00 cb 08
0d: 5c 0c 1f e9 0c
0e: 4c 0c 2d e9 08
0f: 5f 80 03 80 0c
10: 5c 04 00 2b 0c
11: 5f 00 00 cb 00
12: 5f 80 02 a0 0c
13: df d0 00 c0 04
14: 01 3a 00 f3 0a
15: 5c 08 00 b3 0c
16: 5c 0e 00 13 0c
17: 5f 80 00 a0 0c
18: 5c 00 00 13 0c
19: 5c 08 00 b3 0c
1a: 5c 00 00 13 0c
1b: 84 5a 00 b3 0c
1c: 48 0a 5b d1 0c
1d: 5f 80 00 e0 08
1e: 5f 00 1e fd 0c
1f: 5f 80 01 a0 0c
20: df 20 00 cb 08
21: 5c 08 00 b3 0c
22: 5f 80 03 00 0c
23: 5c 08 00 b3 0c
24: 5f 80 00 80 0c
25: 5c 00 00 33 0c
26: 5c 08 00 93 0c
27: 9f 91 ff cf 0e
28: 5c 84 00 20 0c
29: 84 00 00 b3 0c
2a: 49 10 69 d1 0c
2b: 5f 80 00 e0 08
2c: 5f 00 2c fd 0c
2d: 5f 80 01 a0 0c
2e: df 20 00 cb 08
2f: 5c 08 00 b3 0c
30: 5f 80 03 00 0c
31: 5c 00 00 b3 0c
32: 5f 80 01 00 0c
33: 5c 08 00 b3 0c
34: 5f 80 00 80 0c
35: 5c 00 00 33 0c
36: 5c 08 00 93 0c
37: 9f 91 ff cf 0e
38: 5c 84 00 20 0c
39: 84 00 00 b3 0c
3a: 49 10 79 d1 0c
3b: 5f 80 00 e0 08
3c: 5f 00 3c fd 0c
3d: ff ff ff ff ff
3e: ff ff ff ff ff
3f: ff ff ff ff ff

***************************************************************************/

#include "emu.h"
#include "includes/hexion.h"
#include "includes/konamipt.h"

#include "cpu/z80/z80.h"
#include "machine/watchdog.h"
#include "sound/okim6295.h"
#include "sound/k051649.h"

#include "speaker.h"


void hexion_state::coincntr_w(uint8_t data)
{
//logerror("%04x: coincntr_w %02x\n",m_maincpu->pc(),data);

	/* bits 0/1 = coin counters */
	machine().bookkeeping().coin_counter_w(0,data & 0x01);
	machine().bookkeeping().coin_counter_w(1,data & 0x02);

	/* bit 5 = flip screen */
	flip_screen_set(data & 0x20);

	/* other bit unknown */
if ((data & 0xdc) != 0x10) popmessage("coincntr %02x",data);
}

WRITE_LINE_MEMBER(hexion_state::irq_ack_w)
{
	m_maincpu->set_input_line(0, CLEAR_LINE);
}

WRITE_LINE_MEMBER(hexion_state::nmi_ack_w)
{
	m_maincpu->set_input_line(INPUT_LINE_NMI, CLEAR_LINE);
}

void hexion_state::ccu_int_time_w(uint8_t data)
{
	logerror("ccu_int_time rewritten with value of %02x\n", data);
	m_ccu_int_time = data;
}

void hexion_state::hexion_map(address_map &map)
{
	map(0x0000, 0x7fff).rom();
	map(0x8000, 0x9fff).bankr("bank1");
	map(0xa000, 0xbfff).ram();
	map(0xc000, 0xdffe).rw(FUNC(hexion_state::bankedram_r), FUNC(hexion_state::bankedram_w));
	map(0xdfff, 0xdfff).w(FUNC(hexion_state::bankctrl_w));
	map(0xe000, 0xe000).noprw();
	map(0xe800, 0xe8ff).m("k051649", FUNC(k051649_device::scc_map));
	map(0xf000, 0xf00f).rw(m_k053252, FUNC(k053252_device::read), FUNC(k053252_device::write));
	map(0xf200, 0xf200).w("oki", FUNC(okim6295_device::write));
	map(0xf400, 0xf400).portr("DSW1");
	map(0xf401, 0xf401).portr("DSW2");
	map(0xf402, 0xf402).portr("P1");
	map(0xf403, 0xf403).portr("P2");
	map(0xf440, 0xf440).portr("DSW3");
	map(0xf441, 0xf441).portr("SYSTEM");
	map(0xf480, 0xf480).w(FUNC(hexion_state::bankswitch_w));
	map(0xf4c0, 0xf4c0).w(FUNC(hexion_state::coincntr_w));
	map(0xf500, 0xf500).w(FUNC(hexion_state::gfxrom_select_w));
	map(0xf540, 0xf540).r("watchdog", FUNC(watchdog_timer_device::reset_r));
}

void hexion_state::hexionb_map(address_map &map)
{
	map(0x0000, 0x7fff).rom();
	map(0x8000, 0x9fff).bankr("bank1");
	map(0xa000, 0xbfff).ram();
	map(0xc000, 0xdffe).rw(FUNC(hexion_state::bankedram_r), FUNC(hexion_state::bankedram_w));
	map(0xdfff, 0xdfff).w(FUNC(hexion_state::bankctrl_w));
	map(0xe000, 0xe000).noprw();
	map(0xe800, 0xe87f).noprw(); // all the code to use the k051649 is still present
	map(0xe880, 0xe889).noprw(); // but the bootleg has an additional M6295 @ 0xf5c0 instead
	map(0xe88a, 0xe88e).noprw();
	map(0xe88f, 0xe88f).noprw();
	map(0xe8e0, 0xe8ff).noprw();
	map(0xf000, 0xf00f).rw(m_k053252, FUNC(k053252_device::read), FUNC(k053252_device::write));
	map(0xf200, 0xf200).w("oki", FUNC(okim6295_device::write));
	map(0xf400, 0xf400).portr("DSW1");
	map(0xf401, 0xf401).portr("DSW2");
	map(0xf402, 0xf402).portr("P1");
	map(0xf403, 0xf403).portr("P2");
	map(0xf440, 0xf440).portr("DSW3");
	map(0xf441, 0xf441).portr("SYSTEM");
	map(0xf480, 0xf480).w(FUNC(hexion_state::bankswitch_w));
	map(0xf4c0, 0xf4c0).w(FUNC(hexion_state::coincntr_w));
	map(0xf500, 0xf500).w(FUNC(hexion_state::gfxrom_select_w));
	map(0xf540, 0xf540).r("watchdog", FUNC(watchdog_timer_device::reset_r));
	map(0xf5c0, 0xf5c0).w("oki2", FUNC(okim6295_device::write));
}

static INPUT_PORTS_START( hexion )
	PORT_START("DSW1")
	KONAMI_COINAGE_LOC(DEF_STR( Free_Play ), DEF_STR( Free_Play ), SW1)

	PORT_START("DSW2")
	PORT_BIT( 0x0f, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_DIPNAME( 0x70, 0x40, DEF_STR( Difficulty ) )       PORT_DIPLOCATION("SW2:5,6,7")
	PORT_DIPSETTING(    0x70, DEF_STR( Easiest ) )          // "1"
	PORT_DIPSETTING(    0x60, DEF_STR( Very_Easy) )         // "2"
	PORT_DIPSETTING(    0x50, DEF_STR( Easy ) )             // "3"
	PORT_DIPSETTING(    0x40, DEF_STR( Medium ) )           // "4"
	PORT_DIPSETTING(    0x30, DEF_STR( Medium_Hard ) )      // "5"
	PORT_DIPSETTING(    0x20, DEF_STR( Hard ) )             // "6"
	PORT_DIPSETTING(    0x10, DEF_STR( Very_Hard ) )        // "7"
	PORT_DIPSETTING(    0x00, DEF_STR( Hardest ) )          // "8"
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Demo_Sounds ) )      PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("DSW3")
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Flip_Screen ) )      PORT_DIPLOCATION("SW3:1")
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0xfa, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_SERVICE_DIPLOC( 0x04, IP_ACTIVE_LOW, "SW3:3" )

	PORT_START("P1")
	KONAMI8_B12_START(1)

	PORT_START("P2")
	KONAMI8_B12_START(2)

	PORT_START("SYSTEM")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_CUSTOM )   /* 052591? game waits for it to be 0 */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END



static const gfx_layout charlayout =
{
	8,8,
	RGN_FRAC(1,2),
	4,
	{ 0, 1, 2, 3 },
	{ RGN_FRAC(1,2)+0*4, RGN_FRAC(1,2)+1*4, 0*4, 1*4, RGN_FRAC(1,2)+2*4, RGN_FRAC(1,2)+3*4, 2*4, 3*4 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	16*8
};

static GFXDECODE_START( gfx_hexion )
	GFXDECODE_ENTRY( "gfx1", 0, charlayout, 0, 16 )
GFXDECODE_END

TIMER_DEVICE_CALLBACK_MEMBER(hexion_state::scanline)
{
	int scanline = param;

	// z80 /IRQ is connected to the IRQ1(vblank) pin of k053252 CCU
	if(scanline == 256)
		m_maincpu->set_input_line(0, ASSERT_LINE);

	// z80 /NMI is connected to the IRQ2 pin of k053252 CCU
	// the following code is emulating INT_TIME of the k053252, this code will go away
	// when the new konami branch is merged.
	m_ccu_int_time_count--;
	if (m_ccu_int_time_count <= 0)
	{
		m_maincpu->set_input_line(INPUT_LINE_NMI, ASSERT_LINE);
		m_ccu_int_time_count = m_ccu_int_time;
	}
}

void hexion_state::hexion(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, XTAL(24'000'000)/4); /* Z80B 6 MHz @ 17F, xtal verified, divider not verified */
	m_maincpu->set_addrmap(AS_PROGRAM, &hexion_state::hexion_map);
	TIMER(config, "scantimer").configure_scanline(FUNC(hexion_state::scanline), "screen", 0, 1);
	WATCHDOG_TIMER(config, "watchdog");

	K053252(config, m_k053252, XTAL(24'000'000)/2); /* K053252, X0-010(?) @8D, xtal verified, divider not verified */
	m_k053252->int1_ack().set(FUNC(hexion_state::irq_ack_w));
	m_k053252->int2_ack().set(FUNC(hexion_state::nmi_ack_w));
	m_k053252->int_time().set(FUNC(hexion_state::ccu_int_time_w));

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(60);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(0));
	screen.set_size(64*8, 36*8);
	screen.set_visarea(0*8, 64*8-1, 0*8, 32*8-1);
	screen.set_screen_update(FUNC(hexion_state::screen_update));
	screen.set_palette(m_palette);

	GFXDECODE(config, m_gfxdecode, m_palette, gfx_hexion);
	PALETTE(config, "palette", palette_device::RGB_444_PROMS, "proms", 256);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	/* MSM6295GS @ 5E, clock frequency & pin 7 not verified */
	OKIM6295(config, "oki", 1056000, okim6295_device::PIN7_HIGH).add_route(ALL_OUTPUTS, "mono", 0.5);

	/* KONAMI 051649 // 2212P003 // JAPAN 8910EAJ @ 1D, xtal verified, divider not verified */
	K051649(config, "k051649", XTAL(24'000'000)/8).add_route(ALL_OUTPUTS, "mono", 0.5);
}

void hexion_state::hexionb(machine_config &config)
{
	hexion(config);
	m_maincpu->set_addrmap(AS_PROGRAM, &hexion_state::hexionb_map);

	config.device_remove("k051649");

	// clock frequency & pin 7 not verified; this clock and pin 7 being low makes the pitch match the non-bootleg version, so is probably correct
	OKIM6295(config, "oki2", 1056000, okim6295_device::PIN7_LOW).add_route(ALL_OUTPUTS, "mono", 0.5);
}


/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( hexion )
	ROM_REGION( 0x34800, "maincpu", 0 ) /* ROMs + space for additional RAM */
	ROM_LOAD( "122__j_a__b01.16f", 0x00000, 0x20000, CRC(eabc6dd1) SHA1(e74c1f1f2fcf8973f0741a2d544f25c8639448bf) ) /* "122 // J A // B01" @16F (27c010?) */
	ROM_RELOAD(               0x10000, 0x20000 )    /* banked at 8000-9fff */

	ROM_REGION( 0x80000, "gfx1", 0 )    /* addressable by the main CPU */
	ROM_LOAD( "122a07.1h",   0x00000, 0x40000, CRC(22ae55e3) SHA1(41bdc990f69416b639542e2186a3610c16389063) ) /* Later pcbs have mask roms labeled: "KONAMI // 055066 // 122A07 // 233505" @1H (maybe mismarked 2H on pcb?) */
	ROM_LOAD( "122a06.1g",   0x40000, 0x40000, CRC(438f4388) SHA1(9e23805c9642a237daeaf106187d1e1e0692434d) ) /* Later pcbs have mask roms labeled: "KONAMI // 055065 // 122A06 // 233506" @1G */

	ROM_REGION( 0x40000, "oki", 0 ) /* OKIM6295 samples */
	ROM_LOAD( "122a05.2f",   0x0000, 0x40000, CRC(bcc831bf) SHA1(c3382065dd0069a4dc0bde2d9931ec85b0bffc73) ) /* Later pcbs have mask roms labeled: "KONAMI // 055064 // 122A05 // 233507" @2F (maybe 2G? marking isn't visible in the picture I have) */

	ROM_REGION( 0x0300, "proms", 0 )
	ROM_LOAD( "122a04.10b",   0x0000, 0x0100, CRC(506eb8c6) SHA1(3bff7cf286942d8bdbc3998245c3de20981fbecb) ) // AMD27S21 == 82S129
	ROM_LOAD( "122a03.11b",   0x0100, 0x0100, CRC(590c4f64) SHA1(db4b34f8c5fdfea034a94d65873f6fb842f123e9) ) // AMD27S21 == 82S129
	ROM_LOAD( "122a02.13b",   0x0200, 0x0100, CRC(5734305c) SHA1(c72e59acf79a4db1a5a9d827eef899c0675336f2) ) // AMD27S21 == 82S129

	// there are also two PALs of unknown type on the pcb:
	//054843 @12F
	//054844 @12H(12I?)
ROM_END

ROM_START( hexionb )
	ROM_REGION( 0x34800, "maincpu", 0 ) /* ROMs + space for additional RAM */
	ROM_LOAD( "hexionb.u2", 0x00000, 0x20000, CRC(93edc5d4) SHA1(d14c5be85a67eebddda9103bdf19de8c3c05d3af) )
	ROM_RELOAD(               0x10000, 0x20000 )    /* banked at 8000-9fff */

	ROM_REGION( 0x80000, "gfx1", 0 )    /* addressable by the main CPU */
	ROM_LOAD( "hexionb.u30",   0x00000, 0x40000, CRC(22ae55e3) SHA1(41bdc990f69416b639542e2186a3610c16389063) ) // == 122a07.1h
	ROM_LOAD( "hexionb.u29",   0x40000, 0x40000, CRC(438f4388) SHA1(9e23805c9642a237daeaf106187d1e1e0692434d) ) // == 122a06.1g

	ROM_REGION( 0x40000, "oki", 0 ) /* OKIM6295 samples */
	ROM_LOAD( "hexionb.u16",   0x0000, 0x40000, CRC(bcc831bf) SHA1(c3382065dd0069a4dc0bde2d9931ec85b0bffc73) ) // == 122a05.2f

	ROM_REGION( 0x40000, "oki2", 0 ) /* OKIM6295 samples */
	ROM_LOAD( "hexionb.u18",   0x0000, 0x40000, CRC(c179d315) SHA1(b39d5ec8a90b7ae06763191b8324f32fe1d0ca9b) )

	ROM_REGION( 0x0300, "proms", 0 )
	ROM_LOAD( "82s129.u36",   0x0000, 0x0100, CRC(506eb8c6) SHA1(3bff7cf286942d8bdbc3998245c3de20981fbecb) )
	ROM_LOAD( "82s129.u37",   0x0100, 0x0100, CRC(590c4f64) SHA1(db4b34f8c5fdfea034a94d65873f6fb842f123e9) )
	ROM_LOAD( "82s129.u38",   0x0200, 0x0100, CRC(5734305c) SHA1(c72e59acf79a4db1a5a9d827eef899c0675336f2) )

	// there are also two PALs on the bootleg:
	//PAL20L10 @U12
	//PAL20L10 @U31
ROM_END

GAME( 1992, hexion,  0,      hexion,  hexion, hexion_state, empty_init, ROT0, "Konami",                     "Hexion (Japan ver JAB)",         0 )
GAME( 1992, hexionb, hexion, hexionb, hexion, hexion_state, empty_init, ROT0, "bootleg (Impeuropex Corp.)", "Hexion (Asia ver AAA, bootleg)", 0 ) // we're missing an original Asia AAA
