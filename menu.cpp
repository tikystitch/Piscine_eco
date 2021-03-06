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

    // OUVERTURE MODE GRAPHIQUE (ouverture fen�tre allegro)
    set_color_depth(desktop_color_depth());

    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED,largeur,hauteur,0,0)!=0)
    {
        allegro_message("probleme mode graphique : %s", allegro_error);
        allegro_exit();
        exit(EXIT_FAILURE);
    }
    show_mouse(screen); // Affiche pointeur de souris en mode allegro
}

void graphe( std::string nom, int *quitter, std::string matrice)
{
    /// Le nom du r�pertoire o� se trouvent les images � charger
    grman::set_pictures_path("pics");

    /// Un exemple de graphe
    Graph g;
    //g.ChargementFichierMatrice(matrice);
    g.make_example(nom);

    /// Vous gardez la main sur la "boucle de jeu"
    /// ( contrairement � des frameworks plus avanc�s )
    while ( !key[KEY_ESC] )
    {

        /// Il faut appeler les m�thodes d'update des objets qui comportent des widgets
        g.update();
        g.ajouter_sommet();
        g.ajouter_Arete();
        g.Suppression();
        g.SuppressionPar_Space();
        g.dynamisme();


        if ( key[KEY_ENTER] )
        {
//            g.afficherMatriceAdj();
            g.Dynamisme();
//            int t;
//            std::cin >> t;
        }

        if ( key[KEY_C])
        {
            if(g.connexite()==true)
            {
                std::cout << "vous avez une forte connexite " << std::endl;
            }
            else
            {
                std::cout << "vous n'avez pas de forte connexite" << std::endl;
            }
        }

        /// Mise � jour g�n�rale (clavier/souris/buffer etc...)
        grman::mettre_a_jour();
        *quitter = 0;
    }

    g.Sauvegarde(nom);
    // g.SauvegardeMatrice(matrice);
    ///Mettre le changement de graphe ici.
}
void menu()
{
    std::string savane, marine, banquise, matrice1, matrice2, matrice3;

    int quitter = 0;
    BITMAP*page;
    BITMAP*fond_menu;
    BITMAP*explications_menu;


    // Initialisation d'allegro avec en param�tre la taille de la bitmap
    grman::init();  //On initialise qu'une seule fois pour avoir qu'une seule fenetre

    page=create_bitmap(SCREEN_W, SCREEN_H);

    // Chargement des diff�rentes map
    fond_menu=load_bitmap("menu.bmp", NULL);
    if (!fond_menu)
    {
        allegro_message("pas pu trouver/charger mon_image.bmp");
        allegro_exit();
        exit(EXIT_FAILURE);
    }

    explications_menu=load_bitmap("expli2.bmp", NULL);
    if (!explications_menu)
    {
        allegro_message("pas pu trouver/charger mon_image.bmp");
        allegro_exit();
        exit(EXIT_FAILURE);
    }
    // On efface et on applique le fond

    do
    {
        clear_bitmap(page);

        blit(fond_menu, page,0,0,0,0, SCREEN_W, SCREEN_H);

        // On affiche le buffer mis � jour
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
            savane="eco1.txt";
            matrice1 = "matrice.txt";
            clear_bitmap(page);
            graphe(savane, &quitter, matrice1);
        }

        if(mouse_b&1 && mouse_x > 215 && mouse_x < 700 && mouse_y > 300 && mouse_y < 390 )
        {
            marine="eco2.txt";
            clear_bitmap(page);
            graphe( marine, &quitter, matrice2);

        }


        if(mouse_b&1 && mouse_x > 100 && mouse_x < 590 && mouse_y > 400 && mouse_y < 490 )
        {
            banquise="eco3.txt";
            clear_bitmap(page);
            graphe( banquise, &quitter, matrice3);
        }

        if(mouse_b&1 && mouse_x > 100 && mouse_x < 590 && mouse_y > 500 && mouse_y < 590 )
        {
            while (!key[KEY_ESC])
            {// clear_bitmap(page);
            blit(explications_menu, page,0,0,0,0, SCREEN_W, SCREEN_H);

            // On affiche le buffer mis � jour
            blit(page, screen, 0,0,0,0, SCREEN_W, SCREEN_H);
            }
        }

    }
    while (quitter == 0);

    exit(EXIT_SUCCESS);
    std::cout << quitter << std::endl;
}
