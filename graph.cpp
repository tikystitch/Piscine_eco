#include "graph.h"
#include <map>

#include <string>
#include <stack>

/***************************************************
                    VERTEX
****************************************************/

/// Le constructeur met en place les �l�ments de l'interface
VertexInterface::VertexInterface(int idx, int x, int y, std::string pic_name, int pic_idx)
{
    // La boite englobante
    m_top_box.set_pos(x, y);
    m_top_box.set_dim(130, 100);
    m_top_box.set_moveable();

    // Le slider de r�glage de valeur
    m_top_box.add_child( m_slider_value );
    m_top_box.set_bg_color(VERTFLUOSOMBRE);         //Modifier le contenant du slide
    m_slider_value.set_range(0.0, 100.0);  // Valeurs arbitraires, � adapter...
    m_slider_value.set_dim(20,80);
    m_slider_value.set_bg_color(BLANC);
    m_slider_value.set_gravity_xy(grman::GravityX::Left, grman::GravityY::Up);

    // Label de visualisation de valeur
    m_top_box.add_child( m_label_value );
    m_label_value.set_gravity_xy(grman::GravityX::Left, grman::GravityY::Down);

    // Une illustration...
    if (pic_name!="")
    {
        m_top_box.add_child( m_img );
        m_img.set_pic_name(pic_name);
        m_img.set_pic_idx(pic_idx);
        m_img.set_gravity_x(grman::GravityX::Right);
    }

    // Label de visualisation d'index du sommet dans une boite
    m_top_box.add_child( m_box_label_idx );
    m_box_label_idx.set_gravity_xy(grman::GravityX::Right, grman::GravityY::Down);
    m_box_label_idx.set_dim(20,12);
    m_box_label_idx.set_bg_color(BLANC);

    m_box_label_idx.add_child( m_label_idx );
    m_label_idx.set_message( std::to_string(idx) );

    m_top_box.add_child( m_croix );
    m_croix.set_bg_color(ROUGE);
    m_croix.set_dim(10, 10); ///A modifier pour l'avoir en haut � droite
    m_croix.set_gravity_xy(grman::GravityX::Right, grman::GravityY::Up);
}


/// Gestion du Vertex avant l'appel � l'interface
void Vertex::pre_update()
{
    if (!m_interface)
        return;

    /// Copier la valeur locale de la donn�e m_value vers le slider associ�
    m_interface->m_slider_value.set_value(m_value);

    /// Copier la valeur locale de la donn�e m_value vers le label sous le slider
    m_interface->m_label_value.set_message( std::to_string( (int)m_value) );
}


/// Gestion du Vertex apr�s l'appel � l'interface
void Vertex::post_update()
{
    if (!m_interface)
        return;

    /// Reprendre la valeur du slider dans la donn�e m_value locale
    m_value = m_interface->m_slider_value.get_value();
}



/***************************************************
                    EDGE
****************************************************/

/// Le constructeur met en place les �l�ments de l'interface
EdgeInterface::EdgeInterface(Vertex& from, Vertex& to)
{
    // Le WidgetEdge de l'interface de l'arc
    if ( !(from.m_interface && to.m_interface) )
    {
        std::cerr << "Error creating EdgeInterface between vertices having no interface" << std::endl;
        throw "Bad EdgeInterface instanciation";
    }
    m_top_edge.attach_from(from.m_interface->m_top_box);
    m_top_edge.attach_to(to.m_interface->m_top_box);
    m_top_edge.reset_arrow_with_bullet();

    // Une boite pour englober les widgets de r�glage associ�s
    m_top_edge.add_child(m_box_edge);
    m_box_edge.set_dim(24,60);
    m_box_edge.set_bg_color(BLANCBLEU);

    // Le slider de r�glage de valeur
    m_box_edge.add_child( m_slider_weight );
    m_slider_weight.set_range(0.0, 100.0);  // Valeurs arbitraires, � adapter...
    m_slider_weight.set_dim(16,40);
    m_slider_weight.set_gravity_y(grman::GravityY::Up);

    // Label de visualisation de valeur
    m_box_edge.add_child( m_label_weight );
    m_label_weight.set_gravity_y(grman::GravityY::Down);

}


/// Gestion du Edge avant l'appel � l'interface
void Edge::pre_update()
{
    if (!m_interface)
        return;

    /// Copier la valeur locale de la donn�e m_weight vers le slider associ�
    m_interface->m_slider_weight.set_value(m_weight);

    /// Copier la valeur locale de la donn�e m_weight vers le label sous le slider
    m_interface->m_label_weight.set_message( std::to_string( (int)m_weight ) );
}

/// Gestion du Edge apr�s l'appel � l'interface
void Edge::post_update()
{
    if (!m_interface)
        return;

    /// Reprendre la valeur du slider dans la donn�e m_weight locale
    m_weight = m_interface->m_slider_weight.get_value(); // recupere la valeur du slider







}



/***************************************************
                    GRAPH
****************************************************/

/// Ici le constructeur se contente de pr�parer un cadre d'accueil des
/// �l�ments qui seront ensuite ajout�s lors de la mise ne place du Graphe
GraphInterface::GraphInterface(int x, int y, int w, int h)
{
    m_top_box.set_dim(1000,740);
    m_top_box.set_gravity_xy(grman::GravityX::Right, grman::GravityY::Up);

    m_top_box.add_child(m_tool_box);
    m_tool_box.set_dim(80,720);
    m_tool_box.set_gravity_xy(grman::GravityX::Left, grman::GravityY::Up);
    m_tool_box.set_bg_color(BLANCBLEU);

    m_top_box.add_child(m_main_box);
    m_main_box.set_dim(908,720);
    m_main_box.set_gravity_xy(grman::GravityX::Right, grman::GravityY::Up);
    m_main_box.set_bg_color(BLANCJAUNE);
}

void Graph::ChargementFichier( std::string nomfichier)
{
    int nbsommet=0,nbArete=0,sommet=0,x=0,y=0, entrant =0,sortant=0;
    std::string nomFichierImage;
    double valeur, poids;

    std::ifstream fichier(nomfichier, std::ios::in);
    if (!fichier)
    {
        std::cout << "Erreur ! Le fichier n'est pas ouvert" << std::endl;
    }
    else
    {
        fichier >> nbsommet;
        m_nbSommet = nbsommet;
        fichier >> nbArete;
        for( int i =0 ; i<nbsommet; ++i)
        {
///         #num/sommet #valeur du truc � c�t� #| #-> #nom fichier
            fichier >> sommet >> valeur >> x >> y >> nomFichierImage;
            add_interfaced_vertex(sommet,valeur, x, y,nomFichierImage);
        }

        for( int j=0; j< nbArete; ++j)
        {
            fichier >> sommet >> entrant >> sortant >> poids;
            add_interfaced_edge(sommet, entrant, sortant, poids);
        }
    }
}

void Graph::Sauvegarde(std::string nom)
{
    /// ouverture en �criture avec effacement du fichier ouvert  getNomEco()
    std::ofstream fichier(nom, std::ios::out | std::ios::trunc);
    if (!fichier)
    {
        std::cout << "Erreur ! Le fichier n'est pas ouvert" << std::endl;
    }
    else
    {

        fichier << m_vertices.size() << std::endl;
        fichier << m_edges.size() << std::endl;

        for (std::map<int, Vertex>::iterator it=m_vertices.begin(); it!=m_vertices.end(); ++it)
        {
            fichier << it->first << ' ';
            fichier << (it->second).getValue()  << ' ';
            fichier << (it->second).m_interface->m_top_box.get_posx() << ' ';
            fichier << (it->second).m_interface->m_top_box.get_posy() << ' ';
            fichier << (it->second).m_interface->m_img.getPicname() <<std::endl;
        }

        for (std::map<int, Edge>::iterator it= m_edges.begin(); it!= m_edges.end(); ++it)
        {
            fichier << (it->first) << ' ';
            fichier << (it->second).getFrom() << ' ';
            fichier << (it->second).getTo() <<' ';
            fichier << (it->second).getPoids() << std::endl;
        }
        fichier.close();
    }

}

/// M�thode sp�ciale qui construit un graphe arbitraire (d�mo)
/// Cette m�thode est � enlever et remplacer par un syst�me
/// de chargement de fichiers par exemple.
/// Bien s�r on ne veut pas que vos graphes soient construits
/// "� la main" dans le code comme �a.
void Graph::make_example (std::string a)
{
    ///Pour modifier la fen�tre globale
    m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);
//    std::string nom; std::cout << "Le nom " << std::endl;
//    std::cin >> nom;
    ChargementFichier(a);
//    // La ligne pr�c�dente est en gros �quivalente � :
//    // m_interface = new GraphInterface(50, 0, 750, 600);
//    ///Agir ici pour modifier les positions de base des interfaces !!!
//    /// Les sommets doivent �tre d�finis avant les arcs
//    // Ajouter le sommet d'indice 0 de valeur 30 en x=200 et y=100 avec l'image clown1.jpg etc...
//    ///         #num/sommet #valeur du truc � c�t� #| #-> #nom fichier
//    add_interfaced_vertex(0, 30.0, 200, 100, "clown1.jpg");
//    add_interfaced_vertex(1, 60.0, 400, 100, "clown2.jpg");
//    add_interfaced_vertex(2,  50.0, 200, 300, "clown3.jpg");
//    add_interfaced_vertex(3,  0.0, 400, 300, "clown4.jpg");
//    add_interfaced_vertex(4,  100.0, 600, 300, "clown5.jpg");
//    add_interfaced_vertex(5,  50.0, 100, 500, "bad_clowns_xx3xx.jpg", 0);
//    add_interfaced_vertex(6,  0.0, 300, 500, "bad_clowns_xx3xx.jpg", 1);
//    add_interfaced_vertex(7,  0.0, 500, 500, "bad_clowns_xx3xx.jpg", 2);
//
//    /// Les arcs doivent �tre d�finis entre des sommets qui existent !
//    // AJouter l'arc d'indice 0, allant du sommet 1 au sommet 2 de poids 50 etc...
//    add_interfaced_edge(0, 1, 2, 50.0);
//    add_interfaced_edge(1, 0, 1, 50.0);
//    add_interfaced_edge(2, 1, 3, 75.0);
//    add_interfaced_edge(3, 4, 1, 25.0);
//    add_interfaced_edge(4, 6, 3, 25.0);
//    add_interfaced_edge(5, 7, 3, 25.0);
//    add_interfaced_edge(6, 3, 4, 0.0);
//    add_interfaced_edge(7, 2, 0, 100.0);
//    add_interfaced_edge(8, 5, 2, 20.0);
//    add_interfaced_edge(9, 3, 7, 80.0);

}


void Graph::Supp_Sommet_Arete(int idx)
{
    int test=0;
    /// r�f�rence vers le Edge � enlever
    Vertex &remed=m_vertices.at(idx);

    /// test : on a bien des �l�ments interfac�s
    if (m_interface && remed.m_interface)
    {
        /// Ne pas oublier qu'on a fait �a � l'ajout de l'arc :
        /* EdgeInterface *ei = new EdgeInterface(m_vertices[id_vert1], m_vertices[id_vert2]); */
        /* m_interface->m_main_box.add_child(ei->m_top_edge);  */
        /* m_edges[idx] = Edge(weight, ei); */
        /// Le new EdgeInterface ne n�cessite pas de delete car on a un shared_ptr
        /// Le Edge ne n�cessite pas non plus de delete car on n'a pas fait de new (s�mantique par valeur)
        /// mais il faut bien enlever le conteneur d'interface m_top_edge de l'arc de la main_box du graphe
        //  VertexInterface *vi = new VertexInterface(idx, x, y, pic_name, pic_idx);
        //  Ajout de la top box de l'interface de sommet
        //  m_interface->m_main_box.add_child(vi->m_top_box);
        m_interface->m_main_box.remove_child( remed.m_interface->m_top_box );
    }


    ///Faut aussi rajouter l'ar�te + interface arete.

    ///Il faut trouver les arete associ� aux sommets gr�ce au m_to et m_from
    for ( std::map<int,Edge>::iterator it = m_edges.begin(); it!=  m_edges.end(); ++it )
    {
        if( (it->second).getTo() == idx || (it->second).getFrom() == idx )
        {
            test_remove_edge (it->first);
        }
    }

    for (unsigned int i = 0; i<m_matriceadjacence.size(); ++i)
    {
        if ( m_matriceadjacence[i][0] == idx) //index = ligne de la matrice
        {
            m_matriceadjacence.erase(m_matriceadjacence.begin()+i);
        }
    }

    ///Supprime le sommet
    if ( test =0)
        for (std::map<int,Vertex>::iterator it=m_vertices.begin(); it != m_vertices.end(); ++it)
        {
            if( it->first == idx)
            {
                test = 1;
                m_vertices.erase(idx); ///Effacement par cl�
            }
        }
}

/// eidx index of edge to remove
void Graph::test_remove_edge(int eidx)
{
    /// r�f�rence vers le Edge � enlever
    Edge &remed=m_edges.at(eidx);

    std::cout << "Removing edge " << eidx << " " << remed.m_from << "->" << remed.m_to << " " << remed.m_weight << std::endl;

    /// Tester la coh�rence : nombre d'arc entrants et sortants des sommets 1 et 2
    std::cout << m_vertices[remed.m_from].m_in.size() << " " << m_vertices[remed.m_from].m_out.size() << std::endl;
    std::cout << m_vertices[remed.m_to].m_in.size() << " " << m_vertices[remed.m_to].m_out.size() << std::endl;
    std::cout << m_edges.size() << std::endl;

    /// test : on a bien des �l�ments interfac�s
    if (m_interface && remed.m_interface)
    {
        m_interface->m_main_box.remove_child( remed.m_interface->m_top_edge );
    }

    /// Il reste encore � virer l'arc supprim� de la liste des entrants et sortants des 2 sommets to et from !
    /// References sur les listes de edges des sommets from et to
    std::vector<int> &vefrom = m_vertices[remed.m_from].m_out;
    std::vector<int> &veto = m_vertices[remed.m_to].m_in;
    vefrom.erase( std::remove( vefrom.begin(), vefrom.end(), eidx ), vefrom.end() );
    veto.erase( std::remove( veto.begin(), veto.end(), eidx ), veto.end() );

    ///Pour actualiser la matrice d'adjacence
    for ( unsigned int i =0; i< m_matriceadjacence.size(); ++i)
    {
        ///id_vert1
        if ( m_matriceadjacence[i][0] == m_edges[eidx].getFrom() )
        {
            for (unsigned int j =0; j < m_matriceadjacence.size(); ++j )
            {
                ///id_vert2
                if ( m_matriceadjacence[j][0] == m_edges[eidx].getTo())
                {
                    m_matriceadjacence[i][j+1] = 0 ;
                    m_matriceadjacence[j][i+1] = 0 ;
                }
            }
        }
    }



    /// Le Edge ne n�cessite pas non plus de delete car on n'a pas fait de new (s�mantique par valeur)
    /// Il suffit donc de supprimer l'entr�e de la map pour supprimer � la fois l'Edge et le EdgeInterface
    /// mais malheureusement ceci n'enlevait pas automatiquement l'interface top_edge en tant que child de main_box !
    m_edges.erase( eidx );




//    /// Tester la coh�rence : nombre d'arc entrants et sortants des sommets 1 et 2
//    std::cout << m_vertices[remed.m_from].m_in.size() << " " << m_vertices[remed.m_from].m_out.size() << std::endl;
//    std::cout << m_vertices[remed.m_to].m_in.size() << " " << m_vertices[remed.m_to].m_out.size() << std::endl;
//    std::cout << m_edges.size() << std::endl;

}

void Graph::Suppression()
{
    for (std::map<int,Vertex>::iterator it= m_vertices.begin(); it!= m_vertices.end(); ++it)
    {
        if ((it->second).m_interface->m_croix.getVerifSupp() == true)
        {
            Supp_Sommet_Arete(it->first);
        }
    }
}


void Graph::SuppressionPar_Space()
{
    for ( std::map<int,Vertex>::iterator it= m_vertices.begin(); it != m_vertices.end(); ++it) //regarde tous les sommets
    {
        if((it->second).m_interface->m_top_box.getSuppressionPar_click() == true) //si la variable suppression de 'interface du sommet = true
        {
            for ( std::map<int,Vertex>::iterator op= m_vertices.begin(); op != m_vertices.end(); ++op) //je regarde tous les vecteurs de sommets
            {
                if(((op->second).m_interface->m_top_box.getSuppressionPar_click() == true) && op->first != it->first)
                {
                    for (std::map<int, Edge>::iterator ito= m_edges.begin(); ito != m_edges.end(); ++ito) //parcours les aretes pour voir laquelle est adjacente � celle la
                    {
                        if ( (ito->second).getFrom() == it->first && (ito->second).getTo() == op->first) //si on en trouve une on le supprime
                        {
                            test_remove_edge(ito->first);


                            for(std::map<int, Vertex>::iterator boubou= m_vertices.begin(); boubou != m_vertices.end(); ++boubou )
                            {
                                (boubou->second).m_interface->m_top_box.setSuppArete(false); //reset tous les sommets pour leur variable de l interface qui soit egale a faux
                                (boubou->second).m_interface->m_top_box.setSuppArete(false); //evite de garder en memoire
                            }
                        }
                    }
                }
            }
        }
    }

}

/// La m�thode update � appeler dans la boucle de jeu pour les graphes avec interface
void Graph::update()
{
    if (!m_interface)
        return;

    for (auto &elt : m_vertices)
        elt.second.pre_update();

    for (auto &elt : m_edges)
        elt.second.pre_update();

    m_interface->m_top_box.update();

    for (auto &elt : m_vertices)
        elt.second.post_update();

    for (auto &elt : m_edges)
        elt.second.post_update();

    ///Utiliser les edges
//    for (std::map<int, Vertex>::iterator it= m_vertices.begin(); it != m_vertices.end(); ++it)
//    for (unsigned int i=0; i<m_vertices.size(); ++i)
//    {
//            if ( m_matriceadjacence[i][0] == it->first)
//            {
//                for(unsigned int j = 1; j<m_vertices.size(); ++j)
//                {
//
//                }
//
//            }
//    }

///Pour actualiser la matrice d'adjacence avec les valeurs des edges
    for (std::map<int, Edge>::iterator it= m_edges.begin(); it != m_edges.end(); ++it) //parcourt toutes les aretes
    {
        for ( unsigned int i =0; i< m_matriceadjacence.size(); ++i)
        {
            ///id_vert1
            if ( m_matriceadjacence[i][0] == (it->second).getFrom() ) // localiser le point de depart de l'arete dans la matrice d'adjacence
            {
                for (unsigned int j =0; j < m_matriceadjacence.size(); ++j )
                {
                    ///id_vert2
                    if ( m_matriceadjacence[j][0] == (it->second).getTo()) // localiser le point d arriv�e de l'arete dans la matrice d'adjacence
                    {
                        m_matriceadjacence[i][j+1] = (it->second).getPoids() ; //affecte le poid pour la matrice adjacence pond�r�
                        m_matriceadjacence[j][i+1] = (it->second).getPoids() ;
                    }
                }
            }
        }
    }
}

/// Aide � l'ajout de sommets interfac�s
void Graph::add_interfaced_vertex(int idx, double value, int x, int y, std::string pic_name, int pic_idx )
{
    std::vector < int > stock;
    if ( m_vertices.find(idx)!=m_vertices.end() )
    {
        std::cerr << "Error adding vertex at idx=" << idx << " already used..." << std::endl;
        throw "Error adding vertex";
    }
    // Cr�ation d'une interface de sommet
    VertexInterface *vi = new VertexInterface(idx, x, y, pic_name, pic_idx);
    // Ajout de la top box de l'interface de sommet
    m_interface->m_main_box.add_child(vi->m_top_box);
    // On peut ajouter directement des vertices dans la map avec la notation crochet :
    m_vertices[idx] = Vertex(value, vi);
    std::cout << m_vertices[idx].getValue() << std::endl;
    m_vertices[idx].setPoids(value);

    ///Variable pour remplir la matrice d'adjacence / ajouter le sommet dans la matrice
    stock.push_back(idx); //remplir case 0 en la stockant

    for (unsigned int i =1; i<m_nbSommet; ++i)
    {
        stock.push_back(0); //on va remplir la ligne de 0
    }
    m_matriceadjacence.push_back(stock); //ajouter une nouvelle ligne dans la matrice
    stock.clear();

}

/// Aide � l'ajout d'arcs interfac�s
void Graph::add_interfaced_edge(int idx, int id_vert1, int id_vert2, double weight)
{
    if ( m_edges.find(idx)!=m_edges.end() )
    {
        std::cerr << "Error adding edge at idx=" << idx << " already used..." << std::endl;
        throw "Error adding edge";
    }

    if ( m_vertices.find(id_vert1)==m_vertices.end() || m_vertices.find(id_vert2)==m_vertices.end() )
    {
        std::cerr << "Error adding edge idx=" << idx << " between vertices " << id_vert1 << " and " << id_vert2 << " not in m_vertices" << std::endl;
        throw "Error adding edge";
    }

    EdgeInterface *ei = new EdgeInterface(m_vertices[id_vert1], m_vertices[id_vert2]);
    m_interface->m_main_box.add_child(ei->m_top_edge);
    m_edges[idx] = Edge(weight, ei);
    m_edges[idx].m_from = id_vert1;
    m_edges[idx].m_to = id_vert2;

    m_vertices[id_vert1].m_out.push_back(idx);
    m_vertices[id_vert2].m_in.push_back(idx);

    ///Pour compl�ter la matrice d'adjacence. On la remplit avec les poids des arete
    for ( unsigned int i =0; i< m_matriceadjacence.size(); ++i)
    {
        if ( m_matriceadjacence[i][0] == id_vert1 ) //localiser le sommet de depart
        {
            for (unsigned int j =0; j < m_matriceadjacence.size(); ++j )
            {
                if ( m_matriceadjacence[j][0] == id_vert2 ) //localiser le sommet de arrivee
                {
                    m_matriceadjacence[i][j+1] = weight; //affecter le poids
                    m_matriceadjacence[j][i+1] = weight;
                }
            }
        }
    }
}

void Graph::ajouter_sommet()
{
    std::map<int, Vertex>::iterator it;
    int temp;
    std::vector<int> tempoPourMap; ///Variable temporaire pour le tri

    std::string pic_name="";

    /// trie des longueurs de chaque plus court chemin
    for (std::map<int, Vertex>::iterator it= m_vertices.begin(); it != m_vertices.end(); ++it)
        tempoPourMap.push_back(it->first);

    for (unsigned int i=0; i<tempoPourMap.size(); ++i) //utilisation du tri pour ne pas reutiliser les mm indices qu'au depart
    {
        for (unsigned int j=0; j<tempoPourMap.size(); ++j)
        {
            if (tempoPourMap[i] > tempoPourMap[j])
            {
                temp = tempoPourMap[i];
                tempoPourMap[i] = tempoPourMap[j];
                tempoPourMap[j] = temp;
            }
        }
    }
    for (std::map<int,Vertex>::iterator it= m_vertices.begin(); it!= m_vertices.end(); ++it)
    {
        if ((it->second).m_interface->m_top_box.getCopie() == true)
        {
            for( unsigned int i=0; i<m_matriceadjacence.size();  ++i)
            {
                m_matriceadjacence[i].push_back(0);
            }
            m_nbSommet = m_nbSommet + 1;
            add_interfaced_vertex(tempoPourMap[0]+1,  0.0, 400, 300, ((it->second).m_interface->m_img.getPicname()) );
            (it->second).m_interface->m_top_box.setCopie(false); //on le copie pour ne pas avoir des duplications
        }
    }
}

void Graph::ajouter_Arete()
{
    //std::map<int, Vertex>::iterator it;
    int temp;
    int blindage=0;
    std::vector<int> tempoPourMap; ///Variable temporaire pour le tri
    tempoPourMap.push_back(0);
    for ( std::map<int,Vertex>::iterator it= m_vertices.begin(); it != m_vertices.end(); ++it)
    {
        if ((it->second).m_interface->m_top_box.getFromAjout() == true) // si la variable de l'ajout de sommet de depart de 'interface du sommet est egale a true
        {
            for ( std::map<int,Vertex>::iterator op= m_vertices.begin(); op != m_vertices.end(); ++op)
            {
                if(((op->second).m_interface->m_top_box.getToAjout() == true) && op->first != it->first) // si la variable de l'ajout de sommet d arrivee de 'interface du sommet est egale a true
                {
                    for (std::map<int, Edge>::iterator ito= m_edges.begin(); ito != m_edges.end(); ++ito)
                    {
                        tempoPourMap.push_back(ito->first);
                    }
                    for (unsigned int i=0; i<tempoPourMap.size(); ++i) //pour eviter d'ajouter la meme arete on effectue un tri
                    {
                        for (unsigned int j=0; j<tempoPourMap.size(); ++j)
                        {
                            if (tempoPourMap[i] > tempoPourMap[j])
                            {
                                temp = tempoPourMap[i];
                                tempoPourMap[i] = tempoPourMap[j];
                                tempoPourMap[j] = temp;
                            }
                        }
                    }
                    // evite d'avoir plusieurs aretes dans le meme sens avec les meme sommets
                    for (std::map<int, Edge>::iterator blind= m_edges.begin(); blind != m_edges.end(); ++blind )
                    {
                        if( (blind->second).getFrom() == it->first && (blind->second).getTo() == op->first )
                        {
                            blindage=1;
                            std::cout << "From " << it->first << std::endl;
                            std::cout << "To " << op->first << std::endl;
                        }
                    }
                    if( blindage != 1)
                    {
                        add_interfaced_edge(tempoPourMap[0]+1, it->first, op->first, 50.0);
                    }

                    for(std::map<int, Vertex>::iterator boubou= m_vertices.begin(); boubou != m_vertices.end(); ++boubou)
                    {
                        (boubou->second).m_interface->m_top_box.setFromAjout(false);

                        (boubou->second).m_interface->m_top_box.setToAjout(false);
                    }
                }
            }
            ///Pour assurer la bonne arete
            (it->second).m_interface->m_top_box.setFromAjout(false);

        }
    }
}


void Graph::Dynamisme( )
{
      float t=0;
    double P1=0;
    double K1=0;

    for(std::map<int, Vertex>::iterator premier= m_vertices.begin(); premier != m_vertices.end(); ++premier)
    {
        std::cout << "nuuumm   " << premier->first << std::endl;
        for (std::map<int, Edge>::iterator deuxieme= m_edges.begin(); deuxieme != m_edges.end(); ++deuxieme )
        {
            for (unsigned int i=0; i < ((premier->second).get_Out().size()); ++i)
            {
                std::cout << "out   " << (premier->second).m_out[i] << std::endl;
                std::cout << "From arete   " << (deuxieme->second).getFrom() << std::endl;

                    ///Nous donne l'index du sommet sortant
                if ( (deuxieme->first) ==  (premier->second).get_Out()[i]  )
                {
                    //P1 = P1 + (deuxieme->second).getPoids()*
                    ///Regarde tout les sommets
                    for ( std::map<int, Vertex>::iterator findgetToValue= m_vertices.begin(); findgetToValue != m_vertices.end(); ++findgetToValue)
                    {
                        ///Additione le poids de tout les pr�dateurs de l'individus + verifie si le sommet est bien un pr�dateur
                        ///Permet de retrouver la valeur de l'index du sommet sortant. (deuxieme->second).getTo()
                        if ( (deuxieme->second).getTo() == (findgetToValue->first))
                        {
                            std::cout << "From (findgetToValue->second).getValue() " << (findgetToValue->second).getValue() << std::endl;
                            std::cout << "From (deuxieme->second).getPoids() "<< (deuxieme->second).getPoids() << std::endl;
                            //Pred.push_back((deuxieme->second).getPoids() * (findgetToValue->second).getValue());
                            P1 = P1 + (deuxieme->second).getPoids() * (findgetToValue->second).getValue();
                            //Pred.push_back((deuxieme->second).m_interface->m_slider_weight.get_value() * std::stoi((findgetToValue->second).m_interface->m_label_value.get_message()));
                        }
                    }
                }
            }

            for (unsigned int j=0; j < ((premier->second).get_In().size()); ++j)
            {
                std::cout << "in   " << (premier->second).m_in[j] << std::endl;
                std::cout << "To arete   " << (deuxieme->second).getTo() << std::endl;
                if ((deuxieme->first) ==  (premier->second).m_in[j] )
                {
                     ///Additione le poids de tout ce que l'individus mange
                    for ( std::map<int, Vertex>::iterator findgetToValue= m_vertices.begin(); findgetToValue != m_vertices.end(); ++findgetToValue)
                    {

                        ///Verifie si le sommet est bien ce que mange l'individu.
                        if ((deuxieme->second).getFrom() == (findgetToValue->first) )
                        {
                            std::cout << "To  (findgetToValue->second).getValue() " << (findgetToValue->second).getValue() << std::endl;
                            std::cout << "To  (deuxieme->second).getPoids() "<< (deuxieme->second).getPoids() << std::endl;
                           K1 = K1 + (deuxieme->second).getPoids() * (findgetToValue->second).getValue();
                        }
                    }
                }
            }
        }

        ///Pour �viter de diviser par 0;
        if ( K1 == 0 )
        {
            K1 = 1;
        }

        ///Permet de set le sommet � t+1;
        t = ((premier->second).getValue()) + (((premier->second).getRythmeCroissance())*(premier->second).getValue())*(((1-((premier->second).getValue()))/K1) - 0.01*P1);
        (premier->second).setPoids(t);

        ///Permet de blinder pour ne pas avoir de population n�gative ni trop grande.
        if( t <= 0 )
        {
            t =0;
        }
        if ( t >= 100)
        {
            t= 100;
        }
        ///Reset les valeurs de la pr�dation.
        P1 = 0;
        K1 = 0;
    }
}

void Graph::afficherMatriceAdj()
{
    for (unsigned int i =0; i<m_matriceadjacence.size(); ++i )
    {
        for ( unsigned int j = 0; j< m_matriceadjacence.size(); ++j)
        {
            std::cout << m_matriceadjacence[i][j] << ' ';
        }
        std::cout << std::endl;
    }
}

bool Graph::connexite()
{
    int nb_sommet=m_vertices.size(); // On affecte a ordre le nombre de sommets

    int vertices=0;
    int check=0;

    std::vector<bool> connexe;  // Vecteur de bool
    std::stack<int> pile;       // Pile

    pile.push(vertices);

    for(int i=0; i<nb_sommet; i++)
        connexe.push_back(false);  //On rajoute au vecteur de bool connexe nb_sommet case que l'on met a false

    while(!pile.empty())    // Tant que la pile est pas vide
    {
        vertices=pile.top();
        pile.pop();      // On retire la derniere valeur empilee de la pile
        connexe[vertices]=true;

        for(int i=0; i<nb_sommet; i++)
        {
            if(m_matriceadjacence[vertices][i]==1 && connexe[i]==false)   // On parcourt la matrice d'adjacence et le vecteur connexe
            {
                pile.push(i);      // On empile i
                connexe[i]=true;   // On met le vecteur connexe a true
            }
        }
    }

    for(int i=0; i<connexe.size(); i++)
    {
        if(connexe[i]==false)    // On parcout le vecteur connexe s'il est a faux on incremente check
            check++;
    }

    if(check!=0)
        return false;
    else
        return true;

}

void Graph::dynamisme()  /// "P" pour activer le dynamisme
{
    if(key[KEY_P])
    {
        double D=1;
        for(auto& A:m_vertices)
        {
            for(const auto& B: A.second.m_in)
            {
                for(auto& C:m_edges)
                {
                    D = D + m_vertices[B].m_value*C.second.m_weight;
                    D=D/100;
                }
            }

            if(A.second.m_value>0)
            {
                A.second.m_value = A.second.m_value + 0.0001*A.second.m_value*(1-A.second.m_value/D);  //On diminue le poids de tous les sommets plus rapide si 0.0001 >
            }
        }
    }
}

