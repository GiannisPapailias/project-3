#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
 
int main() {

    int sockfd;  // Περιγραφέας socket για τη σύνδεση με τον server
    struct sockaddr_in serv_addr; // Δομή που περιέχει τη διεύθυνση του server
    char buffer[1024]; // Buffer για αποθήκευση δεδομένων που λαμβάνονται από τον server
    int bytes_received; // Μεταβλητή για το μέγεθος των δεδομένων που λαμβάνονται
;

    srand(time(NULL));

    // Δημιουργία socket επικοινωνίας με τον server
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) { // Έλεγχος για αποτυχία δημιουργίας socket
        perror("Socket creation failed"); // Εμφάνιση μηνύματος λάθους
        exit(EXIT_FAILURE); // Τερματισμός του προγράμματος με αποτυχία
    }

    // Ορισμός των παραμέτρων της διεύθυνσης του server
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address or Address not supported"); // Σφάλμα αν η IP δεν είναι έγκυρη
        close(sockfd); // Κλείσιμο του socket
        exit(EXIT_FAILURE); // Τερματισμός του προγράμματος με αποτυχία
    }

    // Σύνδεση του client με τον server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection to the server failed"); // Σφάλμα αν η σύνδεση αποτύχει
        close(sockfd); // Κλείσιμο του socket
        exit(EXIT_FAILURE); // Τερματισμός του προγράμματος με αποτυχία
    }

    // Ο client στέλνει 10 παραγγελίες τυχαίων προϊόντων στον server
    for (int i = 0; i < 10; i++) {
        int product_id = rand() % NUM_PRODUCTS; // Επιλογή ενός τυχαίου προϊόντος από τα διαθέσιμα

        // Αποστολή του product_id στον server
        if (send(sockfd, &product_id, sizeof(int), 0) < 0) {
            perror("Send failed"); // Σφάλμα αν η αποστολή αποτύχει
            close(sockfd); // Κλείσιμο του socket
            exit(EXIT_FAILURE); // Τερματισμός του προγράμματος με αποτυχία
        }

        printf("Sent product ID: %d\n", product_id);

        memset(buffer, 0, sizeof(buffer)); // Εκκαθάριση του buffer πριν τη λήψη νέων δεδομένων
        bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0); // Λήψη απάντησης από τον server

        if (bytes_received == 0) { // Αν ο server τερματίσει τη σύνδεση
            printf("Server closed the connection.\n"); // Εκτύπωση μηνύματος αποσύνδεσης
            break; // Τερματισμός του βρόχου αποστολής παραγγελιών
        } else if (bytes_received < 0) { // Αν η λήψη αποτύχει
            perror("Receive failed"); // Εμφάνιση μηνύματος απουχίας
            close(sockfd); // Κλείσιμο του socket
            exit(EXIT_FAILURE); // Τερματισμός του προγράμματος με αποτυχία
        }

        printf("Server: %s\n", buffer);
        sleep(1);
    }

    close(sockfd);
    printf("Server closed.\n");
    return 0;
}
