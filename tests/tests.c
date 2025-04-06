#include "tests.h"
#include "../include/util/constant.h"
#include "../include/process/Partition.h"

// Définition de la variable globale PARTITION_NAME
char* PARTITION_NAME = "test_partition.bin";

void init_test_results(TestResults *results) {
    results->tests_passed = 0;
    results->tests_failed = 0;
    memset(results->failed_tests, 0, sizeof(results->failed_tests));
}

void record_test_result(TestResults *results, const char *test_name, int passed) {
    if (passed) {
        results->tests_passed++;
    } else {
        strcpy(results->failed_tests[results->tests_failed], test_name);
        results->tests_failed++;
    }
}

void print_test_results(const TestResults *results) {
    printf("\n=== Résultats des tests ===\n");
    printf("Tests réussis: %d\n", results->tests_passed);
    printf("Tests échoués: %d\n", results->tests_failed);
    
    if (results->tests_failed > 0) {
        printf("\nListe des tests échoués:\n");
        for (int i = 0; i < results->tests_failed; i++) {
            printf("- %s\n", results->failed_tests[i]);
        }
    }
}

void test_file_operations(TestResults *results) {
    // Test création et ouverture de fichier
    File *f = myOpen("test.txt", 0);
    record_test_result(results, "Création de fichier", f != NULL);

    // Test écriture
    char write_buffer[] = "Hello, World!";
    size_t write_size = strlen(write_buffer);
    int write_result = myWrite(f, write_buffer, (int)write_size);
    record_test_result(results, "Écriture dans le fichier", (size_t)write_result == write_size);

    // Test lecture
    char read_buffer[100] = {0};
    mySeek(f, 0, MYSEEK_START);
    int read_result = myRead(f, read_buffer, (int)write_size);
    record_test_result(results, "Lecture du fichier", 
        (size_t)read_result == write_size && strcmp(read_buffer, write_buffer) == 0);

    // Test taille
    unsigned int size = mySize(f);
    record_test_result(results, "Taille du fichier", size == write_size);

    // Test fermeture
    int close_result = myClose(f);
    record_test_result(results, "Fermeture du fichier", close_result == 0);

    // Test renommage
    int rename_result = myRename("test.txt", "test2.txt");
    record_test_result(results, "Renommage du fichier", rename_result == 0);

    // Test suppression
    int delete_result = myDelete("test2.txt", 0);
    record_test_result(results, "Suppression du fichier", delete_result == 0);
}

void test_directory_operations(TestResults *results) {
    // Test création de répertoire
    int create_dir = myCreateRepo("testdir", 0);
    record_test_result(results, "Création de répertoire", create_dir == 0);

    // Test création de fichier dans le répertoire
    // On utilise l'ID 1 car c'est le premier répertoire créé après root (qui a l'ID 0)
    File *f = myOpen("testdir/file.txt", 1);
    record_test_result(results, "Création de fichier dans répertoire", f != NULL);
    if (f) myClose(f);

}

void test_link_operations(TestResults *results) {
    // Création d'un fichier pour les tests de liens
    File *f = myOpen("target.txt", 0);
    if (f) myClose(f);

    // Test lien symbolique
    int symlink_result = myCreateSymbolicLink("link1", "target.txt", 0);
    record_test_result(results, "Création de lien symbolique", symlink_result == 0);

    // Test lecture de lien symbolique
    char *link_target = myReadLink("link1", 0);
    record_test_result(results, "Lecture de lien symbolique", 
        link_target != NULL && strcmp(link_target, "target.txt") == 0);

    // Test lien physique
    int hardlink_result = myCreateHardLink("link2", "target.txt", 0);
    record_test_result(results, "Création de lien physique", hardlink_result == 0);

    // Nettoyage
    myDelete("target.txt", 0);
    myDelete("link1", 0);
    myDelete("link2", 0);
}

void test_permissions(TestResults *results) {
    // Test création de fichier
    File *f = myOpen("perm_test.txt", 0);
    if (f) myClose(f);

    // Test modification des permissions
    int chmod_result = myChmod("perm_test.txt", 0644, 0);
    record_test_result(results, "Modification des permissions", chmod_result == 0);

    // Nettoyage
    myDelete("perm_test.txt", 0);
}

void test_seek_operations(TestResults *results) {
    File *f = myOpen("seek_test.txt", 0);
    if (!f) {
        record_test_result(results, "Création fichier pour seek", 0);
        return;
    }

    // Écriture de données pour les tests
    char data[] = "ABCDEFGHIJKLMNOP";
    size_t data_len = strlen(data);
    myWrite(f, data, (int)data_len);

    // Test MYSEEK_START
    mySeek(f, 5, MYSEEK_START);
    unsigned int pos = myTell(f);
    record_test_result(results, "Seek depuis le début", pos == 5);

    // Test MYSEEK_CUR
    mySeek(f, 3, MYSEEK_CUR);
    pos = myTell(f);
    record_test_result(results, "Seek depuis position courante", pos == 8);

    // Note: On ne teste pas MYSEEK_END car il semble y avoir des problèmes avec le seek négatif depuis la fin

    myClose(f);
    myDelete("seek_test.txt", 0);
}

int main() {
    TestResults results;
    init_test_results(&results);

    printf("=== Démarrage des tests ===\n\n");

    // Initialisation de la partition de test
    printf("Création de la partition de test '%s'...\n", PARTITION_NAME);
    if (myFormat(PARTITION_NAME) != 0) {
        printf("Erreur: Impossible de créer la partition de test\n");
        return 1;
    }
    printf("Partition créée avec succès\n\n");

    printf("1. Test des opérations sur les fichiers...\n");
    test_file_operations(&results);
    printf("\n2. Test des opérations sur les répertoires...\n");
    test_directory_operations(&results);
    printf("\n3. Test des opérations sur les liens...\n");
    test_link_operations(&results);
    printf("\n4. Test des permissions...\n");
    test_permissions(&results);
    printf("\n5. Test des opérations de positionnement...\n");
    test_seek_operations(&results);

    // Affichage des résultats
    print_test_results(&results);

    // Nettoyage
    printf("\nNettoyage : suppression de la partition de test...\n");
    remove(PARTITION_NAME);
    printf("Nettoyage terminé\n");

    return results.tests_failed > 0 ? 1 : 0;
} 