/**
 * @mainpage Documentation du Gestionnaire de Fichiers
 *
 * @section intro_sec Introduction
 *
 * Bienvenue dans la documentation du gestionnaire de fichiers ! Ce système implémente
 * un système de fichiers virtuel complet avec les fonctionnalités suivantes :
 * 
 * - Gestion des fichiers et des dossiers et déplacements, via chemins relatifs
 * - Support des liens symboliques et physiques
 * - Système de permissions Unix-like
 * - Gestion efficace de l'espace disque avec bitmap
 * - Sauvegarde et restauration de partitions
 *
 * @section arch_sec Architecture
 *
 * Le système est organisé en plusieurs composants principaux :
 *
 * - @b Super-bloc : Contient les métadonnées globales de la partition
 * - @b Bitmap : Gère l'allocation des blocs de données
 * - @b Table @b des @b fichiers : Stocke les métadonnées des fichiers
 * - @b Table @b des @b dossiers : Maintient la hiérarchie des dossiers
 * - @b Zone @b de @b données : Stocke le contenu des fichiers
 *
 * @section usage_sec Utilisation
 *
 * Le système propose une interface en ligne de commande avec les opérations suivantes :
 *
 * - Création et suppression de fichiers et dossiers
 * - Navigation dans l'arborescence
 * - Lecture et modification des fichiers
 * - Gestion des liens et des permissions
 * - Formatage et visualisation de la partition
 * - Sauvegarde et restauration
 *
 * @section impl_sec Implémentation
 *
 * L'implémentation est organisée en plusieurs modules :
 *
 * - @ref TypeSuperBloc.h "Super-bloc"
 * - @ref TypeBitmap.h "Bitmap"
 * - @ref TypeFichier.h "Gestion des fichiers"
 * - @ref TypeDossier.h "Gestion des dossiers"
 * - @ref Tools.h "Outils et utilitaires"
 *
 * @section files_sec Liste des Fichiers
 *
 * Pour une liste complète des fichiers et leur documentation détaillée,
 * consultez la section <a href="files.html">Liste des Fichiers</a>.
 */