# The Brotherhood — Rekonstrukce systému `actors`, stolů z `vasulka.org` a kontextu instalace

Tento dokument shrnuje rekonstrukci původního řídicího systému **`actors` (UNICOM)** z dochovaného obrazu disku (`archive/`), kompletní přehled stolů z `vasulka.org` (včetně stažené fotodokumentace), vytěžené poznatky z diplomové práce Joeyho Heinena (`docs/ArchivingTheBrotherhood_JoeyH.pdf`) a katalogu `docs/Cat_Brotherhood.pdf`, souvislost s instalací **Light Revisited** na brněnském Špilberku i pozdější osud díla na **Islandu**.

---

## 1. Přehled všech 6 stolů z `vasulka.org`

Na oficiálním archivu [vasulka.org/Woody/Brotherhood/Brotherhood.html](http://vasulka.org/Woody/Brotherhood/Brotherhood.html) (dokumentujícím kompletní premiérové uvedení v **NTT InterCommunication Center [ICC] v Tokiu na podzim 1998**) je série *The Brotherhood* (1990–1998) rozdělena do šesti propojených stolů / interaktivních konstrukcí:

| Stůl | Název (`vasulka.org`) | Původní uzel v `backup_all/` | Stažené obrázky v repozitáři |
| :--- | :--- | :--- | :--- |
| **Table 1** | **Translocations** | `rails` | `assets/vasulka_org/img/Translocations/` (`Brother_overT1.jpg`) |
| **Table 2** | **Automata** | `automata` | `assets/vasulka_org/img/Automata/` (`Brother_overT2.jpg`) |
| **Table 3** | **Friendly Fire** | `fire` | `assets/vasulka_org/img/FriendlyFire/` (`Brother_overT3.jpg`) |
| **Table 4** | **Stealth** | *(ovládáno přes `picdd` / `rails`)* | `assets/vasulka_org/img/Stealth/` (`Brother_overT4.jpg`) |
| **Table 5** | **Scribe** | `scribe` | `assets/vasulka_org/img/Scribe/` (`Brother_overT5.jpg`) |
| **Table 6** | **The Maiden** | `maiden` | `assets/vasulka_org/img/Maiden/` (`Brother_overT6.jpg`) |

Celkem bylo z `vasulka.org` staženo **181 obrázků** ze 136 stránek *The Brotherhood* do [`assets/vasulka_org/`](assets/vasulka_org/) (včetně blokového diagramu `img/blockdiagram.jpg`, přehledových fotografií `PHO_*` a katalogových skenů `CAT_*`).

---

## 2. Rekonstrukce původního disku a architektury `actors` (UNICOM)

### 2.1 Vývoj řídicího softwaru (z `ArchivingTheBrotherhood_JoeyH.pdf`)
Podle technické genealogie Joeyho Heinena (NYU MIAP, 2014) prošel řídicí software *The Brotherhood* třemi generacemi:
1. **INTERCOM** — raná fáze, kde vznikl samotný koncept **„actors“** (nezávislých softwarových agentů reagujících na vstupy a posílajících příkazy do hardwaru).
2. **MIDICOM** — mezistupeň zaměřený na MIDI komunikaci.
3. **UNICOM (1996–1998)** — finální distribuovaná architektura pro Linux (i386), napsaná v C (hlavní autoři **R. Gritzo** a **Bruce Hamilton**), nasazená pro výstavu v ICC Tokio 1998.

### 2.2 Síťová architektura UNICOM (Klient–Server)
Zálohovací skript `archive/backup_all/backup_all.sh` a zdrojové kódy v `archive/unicom driver software/` ukazují, že každý stůl běžel jako samostatný linuxový uzel (`automata`, `fire`, `maiden`, `rails`, `scribe`) spravovaný z centrálního uzlu **`jonah`**.

Systém nestál na jednom monolitickém programu, ale na síťových démonických ovladačích (*device drivers*), se kterými komunikovali drobní klienti zvaní **`actor`** (finální dochovaná verze `actor132.c` z 20. 5. 1998, kompilovaná s `libgdbm`):

| Démon | Výchozí port | Funkce |
| :--- | :--- | :--- |
| **`logicdd`** (`logicdd101.c`) | `9850` | Sdílená tabule globálních proměnných (blackboard) v síti – uchovává např. `%video_mode`, `%mnote`, `%mvel`. |
| **`mididd`** (`mididd150.c`) | `9800` | Čte události z MIDI rozhraní (např. bicí pady Roland) a předává je přihlášeným actorům. |
| **`serialdd`** (`serialdd170`) | `9900` | Sériová linka RS-232 — ovládá přehrávač **LaserDisc** (přímé skoky na snímky, play, reverse). |
| **`opto22dd` / `optodd`** | paralelní port / TCP | Ovládání průmyslových relé Opto 22 (pneumatické ventily, světelné okruhy, projektory). |
| **`picdd`** (`picdd210.c`) | sériová / TCP | Řízení krokových motorů a mikrořadičů PIC (pohony kolejnic, gyroskopy na *Stealth* / *Translocations*). |

### 2.3 Rekonstrukce chování Table 3 (*Friendly Fire*, uzel `10.5.1.15`)
Z dochovaných skriptů v `archive/backup_all/fire/` (`table3_start.sh`, `table3_timeline.sh`, `pad1.sh`–`pad5.sh`, `slide_11.sh`, `p*.sh`) lze chování stolu č. 3 zrekonstruovat do posledního bitu:

1. **Start (`table3_start.sh`):**
   - Spustí úvodní přehrávání videa (`video_pl.sh`), vyčká 15 s a na pozadí spustí nekonečnou hlavní smyčku **`table3_timeline.sh`**.
2. **Dva režimy provozu (`%video_mode` v `logicdd:9850`):**
   - **Video mód (`%video_mode = 1`):**
     - Spouští pět paralelních MIDI actorů **`pad1.sh`–`pad5.sh`** napojených na `mididd` (`10.5.1.15:9800`, kanál 9, noty např. `0x23`).
     - Úder návštěvníka na pad Roland vyhodnotí sílu úhozu (`%mvel` 1–127) a pošle přes `serialdd` (`10.5.1.15:9900`) příkaz LaserDiscu pro okamžitý skok na konkrétní snímek záběrů z války v Zálivu (např. `:1000 se pl\r`, `:3715 se pl\r`, `:8131 se pl\r`, zpětný chod `:64 rb\r`).
   - **Diapozitivový mód (`%video_mode = 0`):**
     - Zabije pad actory (`pads_kill.sh`), přepne pneumatické clony/zrcadla a spustí **`slide_11.sh`**, který v 10s intervalech 11× pulzuje relé pro posun karuselu **35mm diaprojektoru Kodak Carousel**.
3. **Dekódovaná bitová maska paralelního portu (`/dev/lp2`, aktivní v `0`):**
   - Bit 6 (`0x40` = `0`) drží trvalé napájení systému.
   - `\377` (`1111 1111`) = vše vypnuto (`p0-7_off.sh`).
   - `\232` (`1001 1010`) = zapnuty piny **0, 2, 5** (`p0-on_p2-on_p5-on.sh` — video projekce / pneumatika).
   - `\254` (`1010 1100`) = zapnuty piny **0, 1, 4** (`p0-on_p1-on_p4-on.sh` — režim diapozitivů).
   - `\247` (`1010 0111`) = pulz pinů **3 a 4** (`p3-on_p4-on.sh` — posun diapozitivu v karuselu).

V novodobé restauraci z roku 2016 (`LOG.md`) byla nefunkční relé deska Opto 22 nahrazena Arduinem s SSR relé (`relayboard/relayboard.ino`) a LaserDisc přehrávač Raspberry Pi s H.264 videem, přičemž logika zůstala věrná původním `actor` skriptům.

---

## 3. Výstava na Špilberku (2016) a *Light Revisited* (spektrální zrcadla 1/3)

Na podzim 2016 hostil brněnský **hrad Špilberk** (Muzeum města Brna) velkou retrospektivu ***Bohuslav Woody Vasulka: Mystery of Memory*** (v návaznosti na letní restaurátorskou misi v Santa Fe, odkud pochází i tento repozitář).

Součástí špilberské výstavy byla vedle *Friendly Fire* také instalace **Light Revisited (*Light Revisited – Noisefields*)**, která vychází ze slavného experimentálního videa *Noisefields* (1974):
- **Optický princip:** Instalace pracuje se sadou speciálních **dichroických (polopropustných chromatických) zrcadel a projekčních ploch**, z nichž každé **propouští/odráží vždy přibližně 1/3 viditelného spektra (R, G, B)**.
- Světelný svazek z projektoru se tak v prostoru postupně štěpí na červenou, zelenou a modrou složku a vytváří v tmavé místnosti vrstvené, prostorově oddělené pulzující kruhy elektronického šumu synchronizované se zvukem.
- Fotodokumentace z `vasulka.org/Woody/LightRevisited/` (36 obrázků) byla stažena do [`assets/vasulka_org/LightRevisited/`](assets/vasulka_org/LightRevisited/).

---

## 4. Osud instalace na Islandu (závěr)

Dílo *The Brotherhood* bylo v plné šesti-stolové sestavě za života Woodyho Vasulky vystaveno pouze jednou — v roce **1998 v ICC Tokio**. Následně byly jednotlivé stoly rozebrány a uloženy ve skladech v Santa Fe (Rodeo Road, Ruffina Street, Agua Fria Road), kde jim hrozila nevratná degradace kvůli zastaralému hardwaru 90. let, mechanickým relé a vymírající technologii LaserDisc.

Zásadní záchranná a archivní etapa se spojila s rodnou zemí Steiny Vasulky — **Islandem**:
1. **Vasulka Chamber (Národní galerie Islandu, Reykjavík):** Při Národní galerii Islandu vzniklo specializované pracoviště a archiv *Vasulka Chamber*, které dlouhodobě pečuje o digitální i technologický odkaz obou umělců.
2. **Rozsáhlá obnova a výstava v Reykjavíku (2024–2026):** Galerie **BERG Contemporary v Reykjavíku** uspořádala na přelomu let **2024–2026** velkou výstavu ***Woody Vasulka: The Brotherhood***, doprovázenou mezinárodními konzervátorskými sympozii (ve spolupráci s institucemi typu Tate či LACMA). Právě *The Brotherhood* se stalo referenčním příkladem toho, jak lze komplexní *time-based media art* z 90. let zachránit před zánikem kombinací archivace původního kódu (UNICOM / `actors`), emulace a citlivé hardwarové náhrady.
