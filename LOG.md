Approach to Restoration of Friendly Fire

Mise na znovuzprovoznění instalace se uskutečnila 15.7 – 2.8, 2016. Má cesta do Nového Mexika zahrnovala fyzickou kompletaci instalace včetně renovace některých jejích funkčních částí.

Na restaurování Friendly Fire se podílel celý realizační tým pod vedením Steiny a Woodyho Vasulky: Bruce Hamilton, Charles Tuttle, Zevin Polzin, Jakub Hybler, Matěj Strnad a já. Na počátku bylo velmi důležité, že se většina týmu sešla poprvé dohromady a osobně, což umožnilo vyjasnit si velmi rychle některé otázky z předešlé emailové komunikace. Na místě bylo také učiněno několik zásadních rozhodnutí, týkajících se následého procesu restaurování díla. Největší rozpor v předchozí diskuzi vznikl při výběru technologií použtých při renovaci. Mé stanovisko bylo použít co nejvíce z původního – jak softwarového, tak hardwarového – designu, namísto úplné renovace. Jak se později ukázalo, ne vše z původní instalace bylo možné zachránit ve funkčním stavu. Ale byl jsem spokojený, že jsme pro technologické řešení použili výhradně svobodný software, tak jak tomu bylo i v původní verzi.

Na počátku restaurátorské práce bylo nejprve nutné nalézt veškeré díly instalace, které se nacházely na třech různých místech v Santa Fe v Novém Mexiku. Při procesu dohledávání byla velmi nápomocná Steina Vasulka, která ačkoli se na instalaci podle svých slov významně nepodílela, přispívala svou živou pamětí ke kompletaci díla. Většina z dílů instalace se nacházela společně s většinou instalací z cyklu The Brotherhood v pronajatém studiu Vasulkových sídlícím na Rodeo Road. Jednalo se o skladištní prostor, kam byly koncentrovány nesčetné kusy z instalace i další práce. Klíčovým úkolem bylo vytipovat původní součástky instalace a nezaměnit je s podobnými součástkami z instalací jiných. Druhé místo, kde byla nalezena původní projekční plátna, bylo staré studio v tovární hale na Ruffina Street. Tam bylo rovněž zřízeno dočasné pracoviště pro kompletaci všech součástí a testovací provoz. Třetí místo, na kterém se nacházely originální video materiály a některé drobnější součástky jako midi převodníky a chybějící kabely, byl dům Vasulkových na Agua Fria Road. Všechny tři místa byla poměrně snadno dosažitelná, jízdou autem přibližně patnáct minut.

Na Rodeo Road byla samotná konstrukce stolu Friendly Fire společně s většinou funkčních součástek včetně původního pneumatického systému, midi kontroleru a řídící elektronické desky. Mezi těmito součástkami se naštěstí nacházel i pevný disk s operačním systémem Linux, který byl použit v původní instalaci. Tento disk se podařilo přečíst a kompletně zazálohovat. Jeho klon, obsahující operační systém pro architekturu Intel i386, se posléze podařilo zprovoznit pod emulační platofrmou QEMU na současné distribuci Linuxu. Tato emulace se stala vodítkem pro návrh nového softwaru instalace.


Vývoj softwaru se poměrně značně zjednodušil kvůli nezbytné redukci technologií, které jsme se rozhodli oproti původnímu návrhu vyměnit. Jednalo se zejména o technologii optického záznamu analogového videa LaserDisk. Několik původních a funkčních přehrávacích zařízení jsme sice k dispozici měli, chyběl ale původní nosič, jehož opětovná výroba by ale vzhledem k nedostupnosti zaniklé technologie byla příliš náročná.

Brzy po podrobnějším ohledání dostupných součástek se ukázalo, že jedna ze stěžejních, totiž řídící relé deska Opto 22, není v provozuschopném stavu a je třeba ji nahradit modernějším designem. Původní mechanické relé byla nefunkční. Relé a řídící desku jsme nahradili mikrokontrolerem Arduino s optickými SSR (solid state relé). Software pro ovládání včetně finálního designu zpracoval Charles Tuttle. Tím pominula i potřeba využívat původní skripty určené k řízení instalace. Původní řešení obsahovalo robustně zpracovanou paralelní komunikaci se zmíněnou řídící deskou, která byla nahrazena komunikací sériovou přes USB port.
Jelikož jsme neměli k dispozici původní nosič LaserDisc s analogovým videem, samotnou projekci videa jsme nahradili hardwarově akcelerovaným digitálním přehrávačem na platformě Raspberry PI. Tento přehrávač po vzoru původní funkcionality hardwary dovoluje přijímat příkazy z externího kontroleru. Naštěstí se podařilo nalézt původní klon disku v prokládaném DV formátu NTSC, ten byl přizpůsoben nižšímu datovému toku do formátu H264, původní problematické reverzní přehrávání bylo nahrazeno transkódováním videa do zpětného chodu.

Video materiál z původní instalace byl drobně pozměněn. Steina Vasulka se po zhlédnutí dema funkcionality instalace rozhodla zesílit rozhovory ve videu, které jsou pro vyznění celé instalace stěžejní. Dokonce se zde uvažovalo o překladu dialogu, který byl posléze z estetických důvodů zamítnutý.
Rekonstrukcí a náhradou součástí jsme se pokusili co nejvíce přiblížit podobě původní instalace. Většina z viditelných částí zůstala beze změny. Největší změny nastaly v samotném technickém řešení a vnitřku instalace, která je pouhým okem jen těžko pozorovatelná. Soupis změn lze shrnout následovně.

Původní a nezměněné součásti Frindly Fire:
- hliníková konstrukce stolu
- pneumatický systém včetně vývěv, vedení vzduchu a pístů
- pohyblivé součásti, padací clony, polopropustná zrcadla
- diapozitivní projektor Carousel 35mm
- původní translucentní plátna 4x, plátno pro horní projekci 1x
- původní midi-kontroler / midi-pad Roland

Renovované součásti:
- relé board Opto 22 / Arduino SSD relé (v původním boxu)
- řidící PC / mikropočítač Raspberry PI
- přehrávač LaserDisc / mikropočítač Raspberry PI
- analogový data projektor / digitální data projektor s konektivitou HDMI
- stojany na plátna
- reproduktory (zaměnitelné)
- midi převodník na USB

Veškeré původní zdrojové kódy i skripty byly zazálohovány ve veřejně přístupném GIT repozitáři. Nacházejí se zde jednak původní řídící skripty pro parallelní komunikaci s původní deskou Opto 22 i skripty nové, obstarávající komunikaci s mikrokontrolerem Arduino. Veřejná adresa repozitáře se nachází na adrese https://github.com/K0F/FriendlyFire. Na této adrese se budou i nadále koncentrovat současné nezbytné znalosti k znovuzprovoznění instalace. Vznikne zde i podrobný návod pro stavbu instalace a dokumentace konkrétního provedení pro galerii na brněnském Špilberku.
