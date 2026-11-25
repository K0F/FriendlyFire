# FRIENDLY FIRE: TAHÁK PRO PŘEDNÁŠEJÍCÍHO (SPEAKER NOTES)
## Mediální archeologie a resuscitace díla Woodyho a Steiny Vašulkových
### Kryštof Pešek (kof) | Celková stopáž přednášky: 45–55 minut

---

## Časový harmonogram a scénář po slajdech

### [00:00 – 02:00] Slajd 01: Titulní strana & Úvod
- **Cíl:** Rychlé ukotvení tématu – nejde o akademické teoretizování, ale o autentickou zprávu z terénní mediální archeologie.
- **Klíčová myšlenka:** Co dělat, když klíčové dílo světového videoartu leží 18 let v bednách a obecný názor zní, že „už to nikdy nikdo nerozchodí“?
- **Představení kontextu:** Léto 2016 v Santa Fe v Novém Mexiku ➔ podzimní výstava na Špilberku v Muzeu města Brna.

### [02:00 – 05:00] Slajd 02: Krédo mediální archeologie
- **Citace z deníku:** *„Dílo neexistuje, nemá-li kontext.“* (23. 7. 2016).
- **Pointa:** Rozdíl mezi muzeální konzervací (pietní zmrazení, nefunkční hliníkové torzo za sklem) a skutečnou resuscitací (obnova kinetiky, zvuku a vztahu člověk-stroj).
- **Tahák:** Zmínit paradox: kurátoři často raději vystaví mrtvé železo s popiskou „vystaveno jako objekt“, než aby riskovali spálené relé.

### [05:00 – 08:00] Slajdy 03 & 04: The Brotherhood a anatomie Table #3
- **Slajd 03:** Woodyho zlom v 90. letech. Přechod od čistého analogového signálu a Rutt/Etra scan procesoru k těžkým kinetickým stolům. Inspirace Los Alamos a vojenským šrotem. Koncept *Friendly Fire* (střelba do vlastních řad, sebedestrukce techniky).
- **Slajd 04:** Hybridní složení stolu. Zdůraznit střet 3 technologických epoch:
  1. *Pneumatika (těžký průmysl):* kompresor, písty, hliník.
  2. *Optika 80. let:* LaserDisc Pioneer, Kodak Carousel 35mm.
  3. *Výpočetní technika 90. let:* Linux i386, SGI IRIX O2, paralelní port Opto 22, MIDI Roland pad.

### [08:00 – 11:00] Slajd 05: 18 let hibernace (1998 ➔ 2016)
- **Historka:** Po Tokiu (ICC 1998) šel stůl do beden. Všichni předpokládali, že LaserDisc je navždy mrtvý a Opto 22 deska spálená.
- **Tři úrovně smrti:** Hardware mrtvý, software nezkompilovatelný pro moderní jádra, choreografie zapomenutá.

### [11:00 – 16:00] Slajdy 06, 07, 08: Expedice Santa Fe & Steina jako kompas
- **Slajd 06:** Jak k tomu došlo: náhlý telefonát od Jennifer DeFelice (asistentka Tomáše Rullera na FaVU) přes Miloše Vojtěchovského. Rychlé balení a odlet do pouště.
- **Slajd 07:** Tři geografické body v Santa Fe:
  - *Rodeo Road:* Skladiště, prach, neroztříděné bedny všech 6 stolů Brotherhoodu.
  - *Ruffina Street:* Pronajatá tovární hala, provizorní dílna, kompresor, testování.
  - *Agua Fria Road:* Dům Vašulkových, kuchyně, archiv magnetických pásů.
- **Slajd 08:** Steina Vašulková. Říkala: *„I have nothing to do with it, it's Woody's table!“* Ale byla absolutním kompasem. Přesně věděla, ve které krabici leží původní čočky a optická zrcadla.

### [16:00 – 21:00] Slajdy 09, 10, 11, 12: Forenzní digitální archeologie
- **Slajd 09:** Nález pevného disku i386 na dně krabice se zdroji. Okamžitý bitový dump (`ddrescue`).
- **Slajd 10:** Bootování v QEMU. Emulace Linuxu 1997 na moderním laptopu přímo v hale. Oživení původního prostředí.
- **Slajd 11:** Odhalení systému UNICOM (Russell Gritzo, Bruce Hamilton). Pětice démonů: `actor1`, `optodd`, `mididd`, `picdd`, `usc`.
- **Slajd 12:** Rozetská deska `PITT.TXT` (8. března 1997). Časový log 922 sekund (15 minut 22 sekund) – přesný part, kdy clony padají, kdy Carousel cvakne a kam LaserDisc skočí.

### [21:00 – 26:00] Slajdy 13, 14, 15, 16: Restaurátorská etika & Hardwarové náhrady
- **Slajd 13:** Restaurátorská doktrína: *Zachovat tělo a senzorickou zkušenost; nahradit zaniklé orgány otevřeným systémem.*
- **Slajd 14 (Náhrada #1):** Opto 22 ➔ Arduino Uno + Solid State Relays (SSR). Charles Tuttle navrhl miniaturní desku s optickými relé. Konec proudových rázů a jiskření. Přechod z LPT (`\332`) na čitelnou ASCII sériovou linku (`/1100000`).
- **Slajd 15 (Náhrada #2):** LaserDisc ➔ Raspberry Pi s H.264. Nález původního NTSC DV pásku na Agua Fria; deinterlace filtrem `yadif`; GPU dekódování přes `omxplayer`; seekování v reálném čase přes D-Bus (<80 ms).
- **Slajd 16:** Kódový diff v terminálu. 1997 proprietární C vs. 2016 open-source Bash & C.

### [26:00 – 32:00] [VIDEO & AUDIO INTERMEZZO] Slajdy 17, 18, 19: Zásah autorů & Zvukový archiv
- **Pustit video ukázku:** Spustit zkompilovaný terénní střih `assets/friendly_fire_montage.mp4` (cca 65 s autentické montáže a pístů v dílně).
- **Slajd 17 & 18:** Steinino ucho. Když se v hale poprvé rozběhla projekce z RPi, Steina řekla: *„Zastavte to! Industriální dron je moc nahlas. Dialogy vojáků jsou utopené, musíte je vytáhnout do popředí!“* Přemíchání zvuku přímo v terénu.
- **Slajd 19:** Pustit zvukovou ukázku rozhovoru (`vasulka_interview_excerpt.opus`). Woodyho inženýrský humor, vzpomínky na brněnskou průmyslovku a kouření u stolu.

### [32:00 – 38:00] Slajdy 20, 21, 22: Návrat do Brna & Výstava na Špilberku
- **Slajd 20:** 67 kg beden přes oceán (`crates.txt`). Albuquerque ➔ Chicago ➔ Frankfurt ➔ Brno.
- **Slajd 21:** Špilberk 2016. Požadavek na stoprocentní autonomii: systém musel běžet 8 hodin denně bez obsluhy. Skript `run.sh`, ranní start, periodické dýchání pístů, večerní bezpečné parkování.
- **Slajd 22:** Roland MIDI pad. Divák udeří do padu ➔ clony s hlasitým syčením padají k zemi, video skočí na sekvenci zkázy, diaprojektor přehodí snímek. Divák jako operátor zbraňového systému.

### [38:00 – 45:00] Slajdy 23 & 24: Závěry, Otevřenost & Diskuse
- **Slajd 23:** Tři lekce pro mediální archeologii:
  1. *Otevřenost je věčnost:* Pouze open-source hardware a svobodný software zaručí přežití díla.
  2. *Živý autor mění pravidla:* Resuscitace je tvůrčí dialog, nikoliv mumifikace.
  3. *Kód jako partitura:* Git repozitář je samotným notovým záznamem instalace.
- **Slajd 24:** Odkaz na `github.com/k0f/friendlyfire`. Závěrečné poděkování a otevření diskuse.

---

## Pohotovostní historky a odpovědi na záludné dotazy

1. **„Proč jste nesehnali originální LaserDisc přehrávač na eBay?“**  
   *Odpověď:* Na eBay se občas objeví průmyslový Pioneer, ale jeho laserová dioda je na konci životnosti a mechanika podléhá okamžitému opotřebení. Výstava trvala 3 měsíce 8 hodin denně – LaserDisc by odešel během prvních dvou týdnů. Raspberry Pi s H.264 a D-Busem nabídlo spolehlivost na 50 let dopředu s nulovou degradací obrazu.

2. **„Nezměnilo se chování instalace přechodem z Opto 22 na Arduino?“**  
   *Odpověď:* Naopak, zpřesnilo se. Stará relé měla zpoždění a obloukové výboje, které občas restartovaly i386 PC. Polovodičová optická relé (SSR) reagují za zlomek milisekundy a jsou zcela elektricky oddělená.

3. **„Jak reagoval Woody na to, že mu do toho sahá mladý kluk z Brna?“**  
   *Odpověď:* Woody byl v první řadě strojař. Když viděl, že písty dýchají, ventily těsní a clony padají na milimetr přesně podle jeho zápisů z roku 1997, měl obrovskou radost. Pro něj byla mechanická přesnost a poetika zbraní důležitější než to, zda relé spíná sběrnice ISA nebo USB.

---
`--τέχνη & kof`
