Descrierea task-ului/ cerințele:  
   Proiectul implementeaza o variatie a jocului TypeRacer. Jocul porneste din starea de repaus in care utilizatorii isi pot alege modul urmatoarei runde (easy, medium sau hard). La apasarea butonului de Start, led-ul va incepe sa clipeasaca timp de 3 secunde, iar in Terminal va aparea numaratoarea inversa pana la inceputul jocului. Odata inceput jocul, va aparea un cuvant generat aleator, iar utilizatorul va incepe sa tasteze cuvantul respectiv. In cazul in care jucatorul nu are greseli cat timp tasteaza led-ul va ramane verde, dar daca va gresi o litera acesta se va face rosu. Daca reuseste sa scrie cuvantul corect intr-un anumit interval de timp (intervalul de timp depinde de modul in care se afla runda) atunci va aparea un nou cuvant, dar in cazul in care cuvantul nu a fost scris corect in acel interval se va trece la un nou cuvant, iar utilizatorul nu va primi puncte pe acesta. La finalul rundei ce tine 30 de secunde se va afisa scorul obtinut (i.e. numarul de cuvinte scrise corect). Daca in timpul rundei utilizatorul doreste sa o inchida, va putea apasa butonul de Stop, ce coincide cu butonul de Start. (butonul are un rol dublu de a porni si opri runda).  
 
Componentele utilizate:  
•	Arduino UNO (ATmega328P microcontroller)  
•	1 x led RGB ( folosit pentru a sti daca s-a tastat corect sau nu cuvantul)  
•	2 x Butoane ( 1 buton pentru Start/Stop, 1 buton pentru Mod-ul rundei)  
•	5 Rezistoane ( 3 x 330 ohm, 2 x 1000 ohm)  
•	Breadboard  
•	Fire de legatura    

Imagini setup fizic:  
![tema2-montaj1](https://github.com/user-attachments/assets/b5112b28-d0bc-44e9-b764-e523caf3a236)
![tema2-montaj2](https://github.com/user-attachments/assets/827e30c2-5271-4a4c-8a4d-f82059098869)

Link cu functionalitatea montajului fizic:  
https://youtube.com/shorts/iYYe8VuHybc?feature=share   


Schema electrica:  
	Schema electrica a fost realiata in TinkerCAD.  
![image](https://github.com/user-attachments/assets/9cf7a507-046a-4a66-97c5-7da67d5564e3)

