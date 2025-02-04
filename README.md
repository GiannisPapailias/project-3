# **Ηλεκτρονικό Κατάστημα (E-Shop) με Sockets & Διαδικτυακή Επικοινωνία**

## **Περιγραφή Εργασίας**
Αυτή η εργασία αποτελεί την υλοποίηση ενός **ηλεκτρονικού καταστήματος (e-shop)** που επικοινωνεί με **πελάτες μέσω sockets** και επεξεργάζεται **παραγγελίες σε πραγματικό χρόνο**.  

Ο **server** διατηρεί έναν **κατάλογο προϊόντων** και εξυπηρετεί πολλαπλούς **πελάτες (clients)** που υποβάλλουν **αιτήματα αγοράς**.  
Η επικοινωνία γίνεται **μέσω δικτυακών υποδοχών (sockets)**, ενώ η διαχείριση των πελατών υλοποιείται με **διεργασίες (fork) και επικοινωνία μέσω pipes**.

## **Μέθοδοι Υλοποίησης**
**Χρήση Inter-Process Communication (IPC)** μέσω **pipes** για αποθήκευση των αποτελεσμάτων.  
**Δημιουργία και διαχείριση πολλαπλών πελατών** με `fork()`.  
**Ανταλλαγή δεδομένων μεταξύ server και client** μέσω `send()` και `recv()`.  
**Διαχείριση αποθεμάτων προϊόντων** με έλεγχο διαθεσιμότητας.  
**Εκτέλεση αναφορών στο τέλος των συναλλαγών** με στατιστικά στοιχεία.  

---

## **Δομή Εργασίας**

`server.c` : Ο server που επεξεργάζεται παραγγελίες και εξάγει στατιστικά 

`client.c` : Ο client που στέλνει παραγγελίες προς τον server 

`Makefile` : Κανόνες μεταγλώττισης και εκτέλεσης του προγράμματος

---

## **Περιγραφή Λειτουργίας**
**Ο Server ξεκινά** και αρχικοποιεί τον κατάλογο προϊόντων (20 προϊόντα, 2 τεμάχια το καθένα).
**Ο Server συνδέεται στη θύρα `8080`** και δέχεται ταυτόχρονα έως 5 πελάτες.
**Οι Πελάτες συνδέονται** στον **server**, στέλνοντας **10 τυχαίες παραγγελίες** για προϊόντα.
**Ο Server επεξεργάζεται κάθε παραγγελία**:
   - Αν υπάρχει διαθέσιμο απόθεμα, αφαιρείται 1 τεμάχιο και προστίθεται στα έσοδα.
   - Αν το προϊόν είναι εκτός αποθέματος, η παραγγελία αποτυγχάνει.
**Ο Server επιστρέφει απάντηση στον πελάτη** για κάθε παραγγελία.
**Όταν όλοι οι πελάτες ολοκληρώσουν τις παραγγελίες τους**, ο server εμφανίζει **αναφορά με στατιστικά**.

---

## **Αναφορά Στατιστικών**
Στο τέλος της εκτέλεσης, ο **server εκτυπώνει μια αναφορά** με τα εξής δεδομένα:
**Αριθμός αιτημάτων για κάθε προϊόν**  
**Αριθμός επιτυχημένων αγορών**   
**Συνολικός αριθμός παραγγελιών, επιτυχημένων και αποτυχημένων**  
**Συνολικός τζίρος**  
