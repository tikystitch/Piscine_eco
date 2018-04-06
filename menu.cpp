#include <allegro.h>
#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "menu.h"

void lancerToutAllegro(int largeur, int hauteur)
{
    // Initialisation globale d'allegro
    allegro_init();

    // Initialisation clavier
    install_keyboard();

    // Initialisation du son
    install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT,0);

    // Initialisation souris
    if(install_mouse() == -1)
    {
        allegro_message("Erreur d'installation de la souris ! %s", allegro_error);
        allegro_exit();
        exit(EXIT_FAILURE);
    }

    // OUVERTURE MODE GRAPHIQUE (ouverture fenêtre allegro)
    set_color_depth(desktop_color_depth());

    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED,largeur,hauteur,0,0)!=0)
    {
        allegro_message("probleme mode graphique : %s", allegro_error);
        allegro_exit();
        exit(EXIT_FAILURE);
    }
    show_mouse(screen); // Affiche pointeur de souris en mode allegro
}

void graphe( std::string nom, int *quitter)
{
    // On affiche la map principale
    /// A appeler en 1er avant d'instancier des objets graphiques etc...

    /// Le nom du répertoire où se trouvent les images à charger
    grman::set_pictures_path("pics");

    /// Un exemple de graphe
    Graph g;
    g.make_example(nom);

    /// Vous gardez la main sur la "boucle de jeu"
    /// ( contrairement à des frameworks plus avancés )
    while ( !key[KEY_ESC] )
    {

        /// Il faut appeler les méthodes d'update des objets qui comportent des widgets
        g.update();
        g.ajouter_sommet();
        g.Suppression();
        g.SuppressionPar_Space();

        /// Mise à jour générale (clavier/souris/buffer etc...)
        grman::mettre_a_jour();
        *quitter = 0;
    }
    ///Mettre le changement de graphe ici.
    g.Sauvegarde(nom);
}
void menu()
{
    std::string savane, marine, banquise;
    int quitter = 0;
    BITMAP*page;
    BITMAP*fond_menu;


    // Initialisation d'allegro avec en paramètre la taille de la bitmap
    grman::init();  //On initialise qu'une seule fois pour avoir qu'une seule fenetre

    do
    {
        page=create_bitmap(SCREEN_W, SCREEN_H);
        clear_bitmap(page);


        // Chargement des différentes map
        fond_menu=load_bitmap("accueil.bmp", NULL);
        if (!fond_menu)
        {
            allegro_message("pas pu trouver/charger mon_image.bmp");
            allegro_exit();
            exit(EXIT_FAILURE);
        }
        // On efface et on applique le fond
        blit(fond_menu, page,0,0,0,0, SCREEN_W, SCREEN_H);
        textprintf_ex(page,font,50,0,makecol(255, 255, 255),-1,"T R O P H I C  N E T W O R K S");
        textprintf_ex(page,font,300,180,makecol(255, 255, 255),-1,"*** *** M E N U *** ***");
        textprintf_ex(page,font,300,230,makecol(255, 255, 255),-1,"    G R A P H E  1");
        textprintf_ex(page,font,300,280,makecol(255, 255, 255), -1,"   G R A P H E  2");
        textprintf_ex(page,font,300,330,makecol(255, 255, 255), -1,"   G R A P H E  3");
        textprintf_ex(page,font,300,380,makecol(255, 255, 255),-1,"Appuyez sur espace pour quitter le jeu.");
        // On affiche le buffer mis à jour
        blit(page, screen, 0,0,0,0, SCREEN_W, SCREEN_H);



        if(key[KEY_SPACE])
        {
            // On quitte
            quitter =50;
            std::cout << quitter << std::endl;
            allegro_exit();
            exit(EXIT_SUCCESS);
        }

        if(mouse_b&1 && mouse_x > 215 && mouse_x < 590 && mouse_y > 210 && mouse_y < 240 )
        {
           // std::cout << "slfjbozef";
            savane="eco1.txt";
            clear_bitmap(page);
             graphe(savane, &quitter);
        }

        if(mouse_b&1 && mouse_x > 215 && mouse_x < 590 && mouse_y > 250 && mouse_y < 290 )
        {


            marine="eco2.txt";
            clear_bitmap(page);
            graphe( marine, &quitter);
        }



        if(key[KEY_3])
        {
            banquise="eco3.txt";
            clear_bitmap(page);
            graphe( banquise, &quitter);
        }

        if(key[KEY_4])
        {

        }

    }
    while (quitter == 0);

    exit(EXIT_SUCCESS);
    std::cout << quitter << std::endl;
}
