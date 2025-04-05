/**
 * @file Partition.h
 * @author Massin KHELOUFI (100%)
 */
#ifndef PARTITION_H
#define PARTITION_H

/**
 * @brief Formate le disque virtuel en appliquant notre système de fichier
 * <br/> Indication : si le fichier existe déjà, nous vérifions que le fichier
 * nous appartient bien avant de le formater.
 * <br/> Si vous rencontrez des problèmes alors il suffira de manuellement supprimer le fichier
 * @param partitionName nom de la partition
 * @return int 0 en cas de réussite | -1 sinon
 */
int myFormat(char *partitionName);

#endif