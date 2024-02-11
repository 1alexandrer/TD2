#pragma once
// Structures mémoires pour une collection de films.

#include <string>
#include <istream>

struct Film; struct Acteur; // Permet d'utiliser les types alors qu'ils seront défini après.

class ListeFilms {
	public:
	int capacite, nElements;
	Film** elements; // Pointeur vers un tableau de Film*, chaque Film* pointant vers un Film.
	Acteur* trouverActeur(std::string nomActeur) const;
	static ListeFilms creerListe(std::string nomFichier);
	void detruireFilm(Film* film);
	void detruireListeFilm();
	void afficherFilm(const Film& film) const;
	void afficherListeFilms()const;
	void afficherFilmographieActeur(const std::string& nomActeur) const;
	private:
	void ajouterFilm(Film* filmAjout);
	void enleverFilm(Film* filmRetire);
	Acteur* lireActeur(std::istream& fichier);
	Film* lireFilm(std::istream& fichier);
};


struct ListeActeurs {
	int capacite, nElements;
	Acteur** elements; // Pointeur vers un tableau de Acteur*, chaque Acteur* pointant vers un Acteur.
};

struct Film
{
	std::string titre, realisateur; // Titre et nom du réalisateur (on suppose qu'il n'y a qu'un réalisateur).
	int anneeSortie, recette; // Année de sortie et recette globale du film en millions de dollars
	ListeActeurs acteurs;
};

struct Acteur
{
	std::string nom; int anneeNaissance; char sexe;
	ListeFilms joueDans;
};
