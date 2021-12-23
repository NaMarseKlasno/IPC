# Unix1 - IPC 2021

![alt text1][logo]

[logo]: img/os.png "OS"
- ###### procesy vyznačené zvýrazneným okrajom sú programy, ktoré budú pri kontrole zadania dodané. Teda treba vypracovať iba programy P1, P2, T, D a Serv2.


### **POPIS PROCESOV**

**PROCES Zadanie** (vypracovaný študentom, nie je zakreslený v diagrame)

**Spúšťanie**
~~~~
zadanie <číslo portu 1> <číslo portu 2>
~~~~
**Funkcia**

Parametre hlavného programu sú čísla portov pre servery Serv1 (TCP) a Serv2 (UDP). Tieto čísla je potrebné týmto procesom odovzdať. Program Zadanie nech vyhradí všetky zdroje (pre medziprocesovú komunikáciu) a nech spustí všetky procesy. Všetky procesy nech sú realizované ako samostatné programy.

**PROCES Pr**

**Spúšťanie**
~~~~
proc_pr
~~~~
**Funkcia**

Proces Pr pošle signál SIGUSR1 svojmu hlavnému procesu (Zadanie) na signalizáciu, že je pripravený. Proces Pr si údaje bude žiadať:

Ak pošle signál (SIGUSR1) procesu P1, nech tento proces (P1) zapíše slovo prečítané zo súboru p1.txt.
To isté platí aj pre proces P2 a p2.txt. Teda, ak proces Pr pošle signál (SIGUSR1) procesu P2 nech do rúry R1 zapíše slovo proces P2 zo súboru p2.txt.
Proces Pr k slovu prijatému z rúry R1 pridá svoju značku a zapíše nové slovo do rúry R2.


**PROCES S**

**Spúšťanie**
~~~~
proc_s
~~~~
**Funkcia**

Proces S pošle signál SIGUSR1 svojmu hlavnému procesu (Zadanie) na signalizáciu, že je (proces S) pripravený. Proces S príjme slovo zo zdieľanej pamäte SM1 so synchronizáciou semaforom S1 (pozri časť o semaforoch v časti „Popis komunikácie“), pripíše k nemu svoju značku a zapíše ho do zdieľanej pamäte SM2 so synchronizáciou semaforom S2.

**PROCES Serv1**

**Spúšťanie**
~~~~
proc_serv1 <číslo portu 1> <číslo portu 2>
~~~~
**Funkcia**

Proces Serv1 vytvorí TCP server (na porte <číslo portu 1>), ktorý bude prijímať TCP pakety. Server prijaté slová označí svojou značkou a pošle ich ďalej na UDP server (port <číslo portu 2>). Čísla portov 1 a 2 sú argumenty hlavného procesu (pozri kapitolu „PROCES Zadanie“). Proces Serv1 pošle signál SIGUSR1 svojmu hlavnému procesu (Zadanie) na signalizáciu, že je (proces Serv1) pripravený. TCP aj UDP server nech vytvorený na lokálnom počítači, teda na počítači „127.0.0.1“ (pozor, nie „localhost“!).

### POPIS KOMUNIKÁCIE


**Semafor S1**

Pre semafor S1 je potrebné vytvoriť dvojicu semaforov. Proces T nech sa riadi podľa semaforu S1[0] a proces S sa bude riadiť podľa semaforu S1[1], pričom nastavený semafor S1[0] (rozumej nastavený na hodnotu 1) nech znamená, že proces T môže zapisovať do zdieľanej pamäte (SM1). Nastavený semafor S1[1] nech znamená, že proces S môže zo zdieľanej pamäte (SM1) údaje čítať.

**Semafor S2**

Pre semafor S2 je potrebné vytvoriť dvojicu semaforov. Proces S nech sa riadi podľa semaforu S2[0] a proces D sa bude riadiť podľa semaforu S2[1]. Pričom nastavený semafor S2[0] (rozumej nastavený na hodnotu 1) nech znamená, že proces S môže zapisovať do zdieľanej pamäte (SM2). Nastavený semafor S2[1] nech znamená, že proces D môže zo zdieľanej pamäte (SM2) údaje čítať.

**Príklad komunikácie medzi procesom T a procesom S – semafor S1**

Pozn.: Pre semafor S2 je to analogické.

~~~~
Hodnota semaforu               Význam
0                                        červená
1                                        zelená
~~~~

Proces T sa riadi semaforom S1[0] a proces S sa riadi semaforom S1[1]. To znamená, že kým ma proces T na svojom semafore (S1[0]) hodnotu 0 (t.j. červená) – tak stále čaká. To isté platí pre proces S. To znamená, že kým proces S má na svojom semafore (S1[1]) hodnotu 0 (červená) – to znamená, že musí čakať.

Semafor treba inicializovať do nasledovného stavu:

S1[0] = 1 (zelená/môžeš)                                       S1[1] = 0 (červená/stoj)
- tento stav znamená, že proces T môže do zdieľanej pamäte zapisovať. Proces S čaká, má červenú, nemôže čítať.

Proces T má zelenú (teda môže zapisovať), lebo má svoj semafor (S1[0]) nastavený na hodnotu 1 (zelená). Teda do zdieľanej pamäte zapíše svoje údaje. Teraz je potrebné, aby proces T umožnil procesu S údaje čítať. Preto zmení semafory na nasledovný stav:

[Najprv zmení svoj semafor (semafor S1[0]) z hodnoty 1 na hodnotu 0 (teda zo zelenej na červenú) a semafor procesu S (teda S1[1]) zmení z hodnoty 0 na hodnotu 1 (z červenej na zelenú, aby mu naznačil, že údaje sú zapísané a môže ich teda čítať.)]

S1[0] = 0 (červená/stoj)                                         S1[1] = 1 (zelená/môžeš)
- tento stav znamená, že proces T nemôže do zdieľanej pamäte zapisovať, má čakať. Proces S má zelenú, má v zdieľanej pamäti pripravené údaje a môže údaje z pamäte prečítať.

Teraz už môže semafor S zo zdieľanej pamäte údaje prečítať, lebo jeho semafor (S1[1]) sa zmenil z hodnoty 0 (červená) na hodnotu 1 (zelená). Prečíta teda údaje zo zdieľanej pamäte a vymení farby semaforov (t.j. zmení sebe zo zelenej na červenú – z 1 na 0 – a procesu T zmení z červenej na zelenú – z 0 na 1 – aby mohol do pamäte zapisovať).

Súbory p1.txt, p2.txt a serv2.txt

Súbory p1.txt a p2.txt budú obsahovať slová (rozumej reťazce znakov každé v novom riadku), pričom veľkosť slova počas putovania medzi procesmi nepresiahne dĺžku 150 znakov. Súbor serv2.txt nech obsahuje výsledné slová zapísané každé v samostatnom riadku.

**Poznámka:** Pre účely vývoja programov a testovania je možné programy Pr, S a Serv1 stiahnuť zo stránok systému na odovzdávanie zadaní.

**Upozornenie:**

Pozor, nachádzate sa v paralelnom prostredí! To znamená, že poradie vykonávania procesov nemusí byť rovnaké na rôznych systémoch. Preto dbajte na synchronizáciu a vyhnete sa problémom „predbiehania“ procesov!