#pragma region "Includes"//{
#define _CRT_SECURE_NO_WARNINGS // On permet d'utiliser les fonctions de copies de chaînes qui sont considérées non sécuritaires.

#include "structures.hpp"      // Structures de données pour la collection de films en mémoire.

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>




#include "include/bibliotheque_cours.hpp"
#include "include/cppitertools/range.hpp"
#include "include/verification_allocation.hpp" // Nos fonctions pour le rapport de fuites de mémoire.
//#include "include/debogage_memoire.hpp"        // Ajout des numéros de ligne des "new" dans le rapport de fuites.  Doit être après les include du système, qui peuvent utiliser des "placement new" (non supporté par notre ajout de numéros de lignes).
#include "include/gsl/span"

using namespace std;
using namespace iter;
using namespace gsl;

#pragma endregion//}

typedef uint8_t UInt8;
typedef uint16_t UInt16;

#pragma region "Fonctions de base pour lire le fichier binaire"//{
template <typename T>
T lireType(istream& fichier)
{
	T valeur{};
	fichier.read(reinterpret_cast<char*>(&valeur), sizeof(valeur));
	return valeur;
}
#define erreurFataleAssert(message) assert(false&&(message)),terminate()
static const uint8_t enteteTailleVariableDeBase = 0xA0;
size_t lireUintTailleVariable(istream& fichier)
{
	uint8_t entete = lireType<uint8_t>(fichier);
	switch (entete) {
	case enteteTailleVariableDeBase+0: return lireType<uint8_t>(fichier);
	case enteteTailleVariableDeBase+1: return lireType<uint16_t>(fichier);
	case enteteTailleVariableDeBase+2: return lireType<uint32_t>(fichier);
	default:
		erreurFataleAssert("Tentative de lire un entier de taille variable alors que le fichier contient autre chose à cet emplacement.");
	}
}

string lireString(istream& fichier)
{
	string texte;
	texte.resize(lireUintTailleVariable(fichier));
	fichier.read((char*)&texte[0], streamsize(sizeof(texte[0])) * texte.length());
	return texte;
}

#pragma endregion//}

//TODO: Une fonction pour ajouter un Film à une ListeFilms, le film existant déjà; on veut uniquement ajouter le pointeur vers
// le film existant.  Cette fonction doit dubler la taille du tableau alloué, avec au minimum un élément, dans le cas où la
// capacité est insuffisante pour ajouter l'élément.  Il faut alors allouer un nouveau tableau plus grand, copier ce qu'il y
// avait dans l'ancien, et éliminer l'ancien trop petit.  Cette fonction ne doit copier aucun Film ni Acteur, elle doit copier uniquement des pointeurs.
void ListeFilms::ajouterFilm(Film* filmAjout){
	
	
	if (capacite - nElements <= 0){
		int newCapacite = 0;
		if (capacite == 0){
			newCapacite = 1;
		}
				
		else{
			newCapacite = capacite*2;
		}
		Film** newElements = new Film* [newCapacite];

		for(int elem : range(0, nElements)){
			newElements[elem] = elements[elem];
		}
		if (capacite != 0){
			delete[] elements;
		}
		capacite = newCapacite;
		elements = newElements;
	}	
	elements[nElements] = filmAjout;
	nElements += 1;
}	



//TODO: Une fonction pour enlever un Film d'une ListeFilms (enlever le pointeur) sans effacer le film; la fonction prenant en paramètre un pointeur vers le film à enlever.  L'ordre des films dans la liste n'a pas à être conservé.
void ListeFilms::enleverFilm(Film* filmRetire){
	bool  estTrouve = false;
	int index = 0;
	while(!estTrouve){
		if(elements[index] == filmRetire){
			estTrouve = true;
		}
		else{
			index++;
		}
	}

	for(int i : range(index, nElements)){
		elements[i] = elements[i+1]; 
	}
	nElements -= 1;

}
//TODO: Une fonction pour trouver un Acteur par son nom dans une ListeFilms, qui retourne un pointeur vers l'acteur, ou nullptr si l'acteur n'est pas trouvé.  Devrait utiliser span.
Acteur* ListeFilms::trouverActeur(string nomActeur) const{
	for(int i : range(0, nElements)){
		span<Acteur*> acteurSpan(elements[i]->acteurs.elements, elements[i]->acteurs.nElements);
		for(Acteur* acteur : acteurSpan){
			if(acteur->nom == nomActeur){
				
				return acteur;
			}
			
		}
	}
	return nullptr;
}
//TODO: Compléter les fonctions pour lire le fichier et créer/allouer une ListeFilms.  La ListeFilms devra être passée entre les fonctions, pour vérifier l'existence d'un Acteur avant de l'allouer à nouveau (cherché par nom en utilisant la fonction ci-dessus).
Acteur* ListeFilms::lireActeur(istream& fichier)
{
	Acteur acteur = {};
	
	acteur.nom            = lireString(fichier);
	acteur.anneeNaissance = int(lireUintTailleVariable (fichier));
	acteur.sexe           = char(lireUintTailleVariable(fichier));
	Acteur* acteurExiste = trouverActeur(acteur.nom);
	
	if(acteurExiste){
		cout << 77;
		return acteurExiste;
	}
	else{
		
		Acteur* nouvelActeur = new Acteur(acteur);
		cout << nouvelActeur->nom << endl;
		return nouvelActeur;
	}
	return {}; //TODO: Retourner un pointeur soit vers un acteur existant ou un nouvel acteur ayant les bonnes informations, selon si l'acteur existait déjà.  Pour fins de débogage, affichez les noms des acteurs crées; vous ne devriez pas voir le même nom d'acteur affiché deux fois pour la création.
}

Film* ListeFilms::lireFilm(istream& fichier)
{
	Film film = {};
	film.titre       = lireString(fichier);
	film.realisateur = lireString(fichier);
	film.anneeSortie = int(lireUintTailleVariable(fichier));
	film.recette     = int(lireUintTailleVariable(fichier));
	film.acteurs.nElements = int(lireUintTailleVariable(fichier));  
	//NOTE: Vous avez le droit d'allouer d'un coup le tableau
	// pour les acteurs, sans faire de réallocation comme pour 
	//  ListeFilms.  Vous pouvez aussi copier-coller les fonctions
	//   d'allocation de ListeFilms ci-dessus dans des nouvelles 
	//   fonctions et faire un remplacement de Film par Acteur,
	//    pour réutiliser cette réallocation.
	Film* nouveauFilm = new Film(film);
	nouveauFilm->acteurs.elements = new Acteur* [film.acteurs.nElements];
	nouveauFilm->acteurs.capacite = film.acteurs.capacite;
	for (int i = 0; i < film.acteurs.nElements; i++) {
		 //TODO: Placer l'acteur au bon endroit dans les acteurs du film.
		//TODO: Ajouter le film à la liste des films dans lesquels l'acteur joue.
		Acteur* acteur = lireActeur(fichier);
		acteur->joueDans.ajouterFilm(nouveauFilm);
	}
	return nouveauFilm; //TODO: Retourner le pointeur vers le nouveau film.
}

ListeFilms ListeFilms::creerListe(string nomFichier)
{
	ifstream fichier(nomFichier, ios::binary);
	fichier.exceptions(ios::failbit);
	
	int nElements = int(lireUintTailleVariable(fichier));

	//TODO: Créer une liste de films vide.
	ListeFilms nouvelleListe;
	for (int i = 0; i < nElements; i++) {
		Film* film = nouvelleListe.lireFilm(fichier); //TODO: Ajouter le film à la liste.
		nouvelleListe.ajouterFilm(film);
	
	return nouvelleListe; //TODO: Retourner la liste de films.
}
	return {};
}
//TODO: Une fonction pour détruire un film (relâcher toute la mémoire associée à ce film, et les acteurs qui ne jouent plus dans aucun films de la collection).  Noter qu'il faut enleve le film détruit des films dans lesquels jouent les acteurs.  Pour fins de débogage, affichez les noms des acteurs lors de leur destruction.
void ListeFilms::detruireFilm(Film* film){
	enleverFilm(film);
	for(Acteur* acteur : span(film->acteurs.elements, film->acteurs.nElements)){
		if(trouverActeur(acteur->nom) == nullptr){
			delete[] acteur->joueDans.elements;
			delete acteur;
		}
	}
	delete[] film->acteurs.elements;
	delete film;
};
//TODO: Une fonction pour détruire une ListeFilms et tous les films qu'elle contient.
void ListeFilms::detruireListeFilm(){
	
	for(int i = 0; i <= nElements; i++ ){
		detruireFilm(elements[i]);
	}
	delete[] elements;
	capacite = 0;
	nElements = 0;
}
void afficherActeur(const Acteur& acteur)
{
	cout << "  " << acteur.nom << ", " << acteur.anneeNaissance << " " << acteur.sexe << endl;
}

//TODO: Une fonction pour afficher un film avec tous ses acteurs (en utilisant la fonction afficherActeur ci-dessus).
void ListeFilms::afficherFilm(const Film& film) const{
	cout << "  " << film.titre << ", " << film.realisateur << ", " << film.anneeSortie << ", " << film.recette << endl;
	
	for(int i= 0; i <= film.acteurs.nElements; i++){
		//afficherActeur(*film.acteurs.elements[i]);

		cout << film.acteurs.elements[i];
		
	}
}
void ListeFilms::afficherListeFilms() const
{
	//TODO: Utiliser des caractères Unicode pour définir la ligne de séparation (différente des autres lignes de séparations dans ce progamme).
	static const string ligneDeSeparation = "\n\033[35m------------------------------------------\033[0m\n";;
	cout << ligneDeSeparation;
	
	//TODO: Changer le for pour utiliser un span.
	for(Film* film : span(elements, nElements)) {
		//TODO: Afficher le film.
		afficherFilm(*film);
		cout << ligneDeSeparation;
	}
}

void ListeFilms::afficherFilmographieActeur(const string& nomActeur) const
{
	//TODO: Utiliser votre fonction pour trouver l'acteur (au lieu de le mettre à nullptr).
	const Acteur* acteur = trouverActeur(nomActeur);
	if (acteur == nullptr)
		cout << "Aucun acteur de ce nom" << endl;
	else
		afficherListeFilms();
}

int main()
{
	
	bibliotheque_cours::activerCouleursAnsi();  // Permet sous Windows les "ANSI escape code" pour changer de couleurs https://en.wikipedia.org/wiki/ANSI_escape_code ; les consoles Linux/Mac les supportent normalement par défaut.

	//int* fuite = new int; //TODO: Enlever cette ligne après avoir vérifié qu'il y a bien un "Fuite detectee" de "4 octets" affiché à la fin de l'exécution, qui réfère à cette ligne du programme.

	static const string ligneDeSeparation = "\n\033[35m════════════════════════════════════════\033[0m\n";

	//TODO: Chaque TODO dans cette fonction devrait se faire en 1 ou 2 lignes, en appelant les fonctions écrites.

	//TODO: La ligne suivante devrait lire le fichier binaire en allouant la mémoire nécessaire.  Devrait afficher les noms de 20 acteurs sans doublons (par l'affichage pour fins de débogage dans votre fonction lireActeur).
	ListeFilms listeFilms = ListeFilms::creerListe("films.bin");
	cout << 99;
	cout << ligneDeSeparation << "Le premier film de la liste est:" << endl;
	//TODO: Afficher le premier film de la liste.  Devrait être Alien.
	listeFilms.afficherFilm(*listeFilms.elements[0]);
	cout << ligneDeSeparation << "Les films sont:" << endl;
	//TODO: Afficher la liste des films.  Il devrait y en avoir 7.
	listeFilms.afficherListeFilms();
	//TODO: Modifier l'année de naissance de Benedict Cumberbatch pour être 1976 (elle était 0 dans les données lues du fichier).  Vous ne pouvez pas supposer l'ordre des films et des acteurs dans les listes, il faut y aller par son nom.
	listeFilms.trouverActeur("Benedict Cumberbatch")->anneeNaissance = 1976;
	cout << ligneDeSeparation << "Liste des films où Benedict Cumberbatch joue sont:" << endl;
	//TODO: Afficher la liste des films où Benedict Cumberbatch joue.  Il devrait y avoir Le Hobbit et Le jeu de l'imitation.
	listeFilms.afficherFilmographieActeur("Benedict Cumberbatch");
	//TODO: Détruire et enlever le premier film de la liste (Alien).  Ceci devrait "automatiquement" (par ce que font vos fonctions) détruire les acteurs Tom Skerritt et John Hurt, mais pas Sigourney Weaver puisqu'elle joue aussi dans Avatar.
	listeFilms.detruireFilm(listeFilms.elements[0]);
	cout << ligneDeSeparation << "Les films sont maintenant:" << endl;
	//TODO: Afficher la liste des films.
	listeFilms.afficherListeFilms();
	//TODO: Faire les appels qui manquent pour avoir 0% de lignes non exécutées dans le programme (aucune ligne rouge dans la couverture de code; c'est normal que les lignes de "new" et "delete" soient jaunes).  Vous avez aussi le droit d'effacer les lignes du programmes qui ne sont pas exécutée, si finalement vous pensez qu'elle ne sont pas utiles.

	//TODO: Détruire tout avant de terminer le programme.  La bibliothèque de verification_allocation devrait afficher "Aucune fuite detectee." a la sortie du programme; il affichera "Fuite detectee:" avec la liste des blocs, s'il manque des delete.
	listeFilms.detruireListeFilm();
	cout << 104;
}	
