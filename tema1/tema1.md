
# Robotica
Descrierea task-ului/ cerintei:  
   In aceast proiect este implementata o simulare a unei statii de incarcare pentru un vehicul electric, folosind mai mai multe led-uri si butoane. Disponibilitatea statiei trebuie sa fie evidenta prin folosirea unui led ce va avea culoarea verde atunci cand statia este disponibila, iar culoarea led-ului va fi rosie cand in cadrul statiei are loc procesul de incarcare. Apasarea butonului de start va incepe incarcarea statiei, iar gradul de incarcare va fi observat prin cele 4 led-uri care se vor aprinde succesiv la un interval de 3 secunde, reprezentand gradul de incarcare al bateriei, led-ul ce va semnifica procentul curent de incarcare va avea starea de clipire, in timp ce led-urile din urma sa vor fi aprinse, iar celelate stinse. La finalul incarcarii led-urile vor clipi simultan de 3 ori pentru a semnaliza finalizarea procesului. De asemenea, avem optiunea de a incheia fortat incarcarea, atunci cand apasam butonul de stop timp de cel putin o secunda. 
   
Mediu de dezvoltare:  
   Proiectul a fost dezvoltat folosind Visual Studio Code ca editor principal de cod. A fost utilizata extensia PlatformIO, un ecosistem pentru dezvoltare embedded. Placa utilizata este Arduino UNO, iar framework-ul este Arduino.  
   
Arhitectura programului:  
   Pentru realizarea unui cod coerent si lizibil programul a fost realizat in mai multe etape cheie. Este esential ca primul lucru realizat in program sa fie definirea si configurarea pini-lor led-urilor si butoanelor, folosite in proiect. Led-urile reprezinta iesirile digitale, iar butonalele intrarile este de mentionat si nevoia existentei rezistentelor de pull-up necesare pentru o buna functionare a butoanelor. Configurarea pinilor realizandu-se desigur in setup().  
   In loop() are loc partea de gestionare a cerintelor proiectului. Inainte de a realiza logica de gestionare a loader-ului de incarcare este esentiala stabilirea clara a starii in care se afla in acel moment statia, acesta fiind si prima etapa din functia loop(). Initial starea incarcarii statiei este libera, iar la apasarea butonului de start starea acesteia se va modifica fiind ocupata. Pentru a fii siguri ca butonul a fost cu adevarat apasat am folosit debounce. Atunci cand statia este libera led-ul statiei va avea culoarea verde, iar cand este ocupata va fi desigur rosu.	
In cazul in care statia se afla in procesul de incarcare vom permite si optiunea de oprire fortata. Oprirea fortata este permisa doar in cazul in care incarcarea bateriei este activa, iar acesta se va realiza prin apasarea butonului de stop timp de minim o secunda. Pentru a fii siguri ca butonul de stop a fost apasat timp de minim o secunda am folosit, desigur, debouncing.  
   Ne aflam in continuare pe ramura in care butonul de start este apasat, iar incarcarea incepe. In cazul in care butonul de stop nu este apasat vom parcurge pe rand led-urile din loader si vom realiza logica de incarcare a bateriei folosind 4 led-uri ce reprezinta pragurile de incarcare de 25%, 50%, 75% si 100 %.  Led-urile vor fii parcurse pe rand, fiecare led va clipi timp de 3 secunde, clipirea realizandu-se la intervale fixe, iar dupa cele 3 secunde va ramane aprins si se va trece la urmatorul led din loader ce va avea si el la randul sau acelasi comportament. Dupa ce am parcurs toate led-urile, deci bateria este incarcata complet, vom apela functia blinkFinal(). Aceasta functie, blinkFinal() realizeaza comportamentul de clipire simultana de 3 ori a led-urilor din loader. In functie se foloseste delay(), pentru a nu permite inregistrarea unui nou start sau stop in timpul clipirii simultane. Urmatorul pas dupa finalizarea clipirii succesive ce marcheaza finalul va fi sa resetam toate variabilele folosite la starea initiala pentru a permite o urmatoare incarcare. Statia este din nou verde si procesul de incarcare este inactiv.  
   In cazul in care cat timp statia se afla in procesul de incarcare, iar butonul de stop va fi apasat se va apela functia blinkFinal(), iar la fel ca si in cazul unei incarcari ce nu a fost fortata, variabilele vor fii resetate la valorile initiale, iar led-ul statiei va fii din nou verde.  

Componente utilizate:  
  •	4 x LED-uri albastre ( folosite pentru a simula procentul de incarcare a bateriei )  
  •	1 x LED RGB ( culoarea sa rezprezinta disponibiltatea statiei de incarcare)  
  •	2 x Butoane ( un buton pentru start incarcare si unul de stop incarcare)  
  •	2 x Rezistoare de 1K pentru butoane  
  •	7 x Rezistoare de 330 ohm pentru led-uri  
  •	Breadboard  
  •	Linii de legatura  
  •	Placa Arduino UNO R3  

Poze setup:  
![image](https://github.com/user-attachments/assets/d5468c13-5d9d-4510-8ea2-cf67e75974eb)  

![image](https://github.com/user-attachments/assets/079e1bc1-97bc-4e89-a48f-7df835a4061d)

Videoclip cu functionalitatea motajului fizic:  
https://youtube.com/shorts/073dzT4wZzw?feature=share

Schema electrica:  
   Schema electrica a fost realizata in Tinkercad.  
![image](https://github.com/user-attachments/assets/ed76acaa-b215-4c1d-8a04-4fb0df6eff3c)



