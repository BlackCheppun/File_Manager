#ifndef TESTS_H
#define TESTS_H

#include "../include/process/Fichier.h"
#include "../include/process/Partition.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

// Structure pour stocker les résultats des tests
typedef struct {
    int tests_passed;
    int tests_failed;
    char failed_tests[1000][100];  // Stocke les noms des tests échoués
} TestResults;

// Fonctions de test pour chaque fonctionnalité
void test_file_operations(TestResults *results);
void test_directory_operations(TestResults *results);
void test_link_operations(TestResults *results);
void test_permissions(TestResults *results);
void test_seek_operations(TestResults *results);

// Utilitaires
void init_test_results(TestResults *results);
void print_test_results(const TestResults *results);
void record_test_result(TestResults *results, const char *test_name, int passed);

#endif // TESTS_H 