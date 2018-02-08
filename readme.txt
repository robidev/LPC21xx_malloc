Hier de functie malloc voor de LPC2106.
Ook is een voorbeeldje van uC/OSII icm de malloc functie bijgevoegd.

de functie maakt gebruik van een singly linked list, en gebruikt het first-
fit algoritme, waarbij een eventueel overschot beschikbaar blijft. Ook
groeit/krimpt de heap dynamisch, zodat het mogelijk is om ook de stack
dynamisch te laten groeien. Zie malloc.c/h voor verdere uitleg.

Vanwege problemen met het initialiseren van globale en static variabelen
(alles in de bss sectie) moet wel handmatig de functie init_malloc eerst
worden aangeroepen. Het probleem lijkt te zitten in data dat nog in het
flash staat van vorige programma's, en niet is overschreven door het huidige
