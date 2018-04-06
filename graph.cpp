#include "graph.h"
#include <map>

/***************************************************
                    VERTEX
****************************************************/

/// Le constructeur met en place les éléments de l'interface
VertexInterface::VertexInterface(int idx, int x, int y, std::string pic_name, int pic_idx)
{
    // La boite englobante
    m_top_box.set_pos(x, y);
    m_top_box.set_dim(130, 100);
    m_top_box.set_moveable();

    // Le slider de réglage de valeur
    m_top_box.add_child( m_slider_value );
    m_top_box.set_bg_color(VERTFLUOSOMBRE);         //Modifier le contenant du slide
    m_slider_value.set_range(0.0, 100.0);  // Valeurs arbitraires, à adapter...
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
    m_croix.set_dim(10, 10); ///A modifier pour l'avoir en haut à droite
    m_croix.set_gravity_xy(grman::GravityX::Right, grman::GravityY::Up);
}


/// Gestion du Vertex avant l'appel à l'interface
void Vertex::pre_update()
{
    if (!m_interface)
        return;

    /// Copier la valeur locale de la donnée m_value vers le slider associé
    m_interface->m_slider_value.set_value(m_value);

    /// Copier la valeur locale de la donnée m_value vers le label sous le slider
    m_interface->m_label_value.set_message( std::to_string( (int)m_value) );
}


/// Gestion du Vertex après l'appel à l'interface
void Vertex::post_update()
{
    if (!m_interface)
        return;

    /// Reprendre la valeur du slider dans la donnée m_value locale
    m_value = m_interface->m_slider_value.get_value();
}



/***************************************************
                    EDGE
****************************************************/

/// Le constructeur met en place les éléments de l'interface
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

    // Une boite pour englober les widgets de réglage associés
    m_top_edge.add_child(m_box_edge);
    m_box_edge.set_dim(24,60);
    m_box_edge.set_bg_color(BLANCBLEU);

    // Le slider de réglage de valeur
    m_box_edge.add_child( m_slider_weight );
    m_slider_weight.set_range(0.0, 100.0);  // Valeurs arbitraires, à adapter...
    m_slider_weight.set_dim(16,40);
    m_slider_weight.set_gravity_y(grman::GravityY::Up);

    // Label de visualisation de valeur
    m_box_edge.add_child( m_label_weight );
    m_label_weight.set_gravity_y(grman::GravityY::Down);

}


/// Gestion du Edge avant l'appel à l'interface
void Edge::pre_update()
{
    if (!m_interface)
        return;

    /// Copier la valeur locale de la donnée m_weight vers le slider associé
    m_interface->m_slider_weight.set_value(m_weight);

    /// Copier la valeur locale de la donnée m_weight vers le label sous le slider
    m_interface->m_label_weight.set_message( std::to_string( (int)m_weight ) );
}

/// Gestion du Edge après l'appel à l'interface
void Edge::post_update()
{
    if (!m_interface)
        return;

    /// Reprendre la valeur du slider dans la donnée m_weight locale
    m_weight = m_interface->m_slider_weight.get_value();
}



/***************************************************
                    GRAPH
****************************************************/

/// Ici le constructeur se contente de préparer un cadre d'accueil des
/// éléments qui seront ensuite ajoutés lors de la mise ne place du Graphe
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
        fichier >> nbArete;
        for( int i =0 ; i<nbsommet; ++i)
        {
///         #num/sommet #valeur du truc à côté #| #-> #nom fichier
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
    /// ouverture en écriture avec effacement du fichier ouvert  getNomEco()
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


/// Méthode spéciale qui construit un graphe arbitraire (démo)
/// Cette méthode est à enlever et remplacer par un système
/// de chargement de fichiers par exemple.
/// Bien sûr on ne veut pas que vos graphes soient construits
/// "à la main" dans le code comme ça.
void Graph::make_example (std::string a)
{
    ///Pour modifier la fenêtre globale
    m_interface = std::make_shared<GraphInterface>(50, 0, 750, 600);
//    std::string nom; std::cout << "Le nom " << std::endl;
//    std::cin >> nom;
    ChargementFichier(a);
//    // La ligne précédente est en gros équivalente à :
//    // m_interface = new GraphInterface(50, 0, 750, 600);
//    ///Agir ici pour modifier les positions de base des interfaces !!!
//    /// Les sommets doivent être définis avant les arcs
//    // Ajouter le sommet d'indice 0 de valeur 30 en x=200 et y=100 avec l'image clown1.jpg etc...
//    ///         #num/sommet #valeur du truc à côté #| #-> #nom fichier
//    add_interfaced_vertex(0, 30.0, 200, 100, "clown1.jpg");
//    add_interfaced_vertex(1, 60.0, 400, 100, "clown2.jpg");
//    add_interfaced_vertex(2,  50.0, 200, 300, "clown3.jpg");
//    add_interfaced_vertex(3,  0.0, 400, 300, "clown4.jpg");
//    add_interfaced_vertex(4,  100.0, 600, 300, "clown5.jpg");
//    add_interfaced_vertex(5,  50.0, 100, 500, "bad_clowns_xx3xx.jpg", 0);
//    add_interfaced_vertex(6,  0.0, 300, 500, "bad_clowns_xx3xx.jpg", 1);
//    add_interfaced_vertex(7,  0.0, 500, 500, "bad_clowns_xx3xx.jpg", 2);
//
//    /// Les arcs doivent être définis entre des sommets qui existent !
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
    /// référence vers le Edge à enlever
    Vertex &remed=m_vertices.at(idx);

    /// test : on a bien des éléments interfacés
    if (m_interface && remed.m_interface)
    {
        /// Ne pas oublier qu'on a fait ça à l'ajout de l'arc :
        /* EdgeInterface *ei = new EdgeInterface(m_vertices[id_vert1], m_vertices[id_vert2]); */
        /* m_interface->m_main_box.add_child(ei->m_top_edge);  */
        /* m_edges[idx] = Edge(weight, ei); */
        /// Le new EdgeInterface ne nécessite pas de delete car on a un shared_ptr
        /// Le Edge ne nécessite pas non plus de delete car on n'a pas fait de new (sémantique par valeur)
        /// mais il faut bien enlever le conteneur d'interface m_top_edge de l'arc de la main_box du graphe
        //  VertexInterface *vi = new VertexInterface(idx, x, y, pic_name, pic_idx);
        //  Ajout de la top box de l'interface de sommet
        //  m_interface->m_main_box.add_child(vi->m_top_box);
        m_interface->m_main_box.remove_child( remed.m_interface->m_top_box );
    }


    ///Faut aussi rajouter l'arête + interface arete.

    ///Il faut trouver les arete associé aux sommets grâce au m_to et m_from
    for ( std::map<int,Edge>::iterator it = m_edges.begin(); it!=  m_edges.end(); ++it )
    {
        if( (it->second).getTo() == idx || (it->second).getFrom() == idx )
        {
            test_remove_edge (it->first);
        }
    }

    ///Supprime le sommet
    for (std::map<int,Vertex>::iterator it=m_vertices.begin(); it != m_vertices.end(); ++it)
    {
        if( it->first == idx)
        {
            m_vertices.erase(idx); ///Effacement par clé

        }
    }

}

/// eidx index of edge to remove
void Graph::test_remove_edge(int eidx)
{
    /// référence vers le Edge à enlever
    Edge &remed=m_edges.at(eidx);

    std::cout << "Removing edge " << eidx << " " << remed.m_from << "->" << remed.m_to << " " << remed.m_weight << std::endl;

    /// Tester la cohérence : nombre d'arc entrants et sortants des sommets 1 et 2
    std::cout << m_vertices[remed.m_from].m_in.size() << " " << m_vertices[remed.m_from].m_out.size() << std::endl;
    std::cout << m_vertices[remed.m_to].m_in.size() << " " << m_vertices[remed.m_to].m_out.size() << std::endl;
    std::cout << m_edges.size() << std::endl;

    /// test : on a bien des éléments interfacés
    if (m_interface && remed.m_interface)
    {
        m_interface->m_main_box.remove_child( remed.m_interface->m_top_edge );
    }

    /// Il reste encore à virer l'arc supprimé de la liste des entrants et sortants des 2 sommets to et from !
    /// References sur les listes de edges des sommets from et to
    std::vector<int> &vefrom = m_vertices[remed.m_from].m_out;
    std::vector<int> &veto = m_vertices[remed.m_to].m_in;
    vefrom.erase( std::remove( vefrom.begin(), vefrom.end(), eidx ), vefrom.end() );
    veto.erase( std::remove( veto.begin(), veto.end(), eidx ), veto.end() );

    /// Le Edge ne nécessite pas non plus de delete car on n'a pas fait de new (sémantique par valeur)
    /// Il suffit donc de supprimer l'entrée de la map pour supprimer à la fois l'Edge et le EdgeInterface
    /// mais malheureusement ceci n'enlevait pas automatiquement l'interface top_edge en tant que child de main_box !
    m_edges.erase( eidx );


    /// Tester la cohérence : nombre d'arc entrants et sortants des sommets 1 et 2
    std::cout << m_vertices[remed.m_from].m_in.size() << " " << m_vertices[remed.m_from].m_out.size() << std::endl;
    std::cout << m_vertices[remed.m_to].m_in.size() << " " << m_vertices[remed.m_to].m_out.size() << std::endl;
    std::cout << m_edges.size() << std::endl;

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

    //std::vector<int> tempoPourMap; ///Variable temporaire pour le tri

    for ( std::map<int,Vertex>::iterator it= m_vertices.begin(); it != m_vertices.end(); ++it)
    {
        if ((it->second).m_interface->m_top_box.getSuppressionPar_click() == true)
        {
            for ( std::map<int,Vertex>::iterator op= m_vertices.begin(); op != m_vertices.end(); ++op)
            {
                if(((op->second).m_interface->m_top_box.getSuppressionPar_click() == true) && op->first != it->first)
                {
                    for (std::map<int, Edge>::iterator ito= m_edges.begin(); ito != m_edges.end(); ++ito)
                    {
                        if ( (ito->second).getFrom() == it->first || (ito->second).getTo() == op->first)
                        {
                            test_remove_edge(ito->first);
                            (it->second).m_interface->m_top_box.setSuppArete(false);
                            (op->second).m_interface->m_top_box.setSuppArete(false);
                        }
                    }
                }
            }
        }
    }

}

/// La méthode update à appeler dans la boucle de jeu pour les graphes avec interface
void Graph::update()
{
    if (!m_interface)
        return;

    for (auto &elt : m_vertices)
        elt.second.pre_update();

    ajouter_Arete();

    for (auto &elt : m_edges)
        elt.second.pre_update();

    m_interface->m_top_box.update();

    for (auto &elt : m_vertices)
        elt.second.post_update();

    for (auto &elt : m_edges)
        elt.second.post_update();

}

/// Aide à l'ajout de sommets interfacés
void Graph::add_interfaced_vertex(int idx, double value, int x, int y, std::string pic_name, int pic_idx )
{
    if ( m_vertices.find(idx)!=m_vertices.end() )
    {
        std::cerr << "Error adding vertex at idx=" << idx << " already used..." << std::endl;
        throw "Error adding vertex";
    }
    // Création d'une interface de sommet
    VertexInterface *vi = new VertexInterface(idx, x, y, pic_name, pic_idx);
    // Ajout de la top box de l'interface de sommet
    m_interface->m_main_box.add_child(vi->m_top_box);
    // On peut ajouter directement des vertices dans la map avec la notation crochet :
    m_vertices[idx] = Vertex(value, vi);
}

/// Aide à l'ajout d'arcs interfacés
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

    for (unsigned int i=0; i<tempoPourMap.size(); ++i)
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
            add_interfaced_vertex(tempoPourMap[0]+1,  0.0, 400, 300, ((it->second).m_interface->m_img.getPicname()) );
        }
    }

//    std::cout << pic_name << std::endl;
//    if((mouse_b & 1)&&(mouse_x<70 && mouse_y<600 && mouse_x>0 && mouse_y>470))
//    {
//        //add_interfaced_vertex(m_vertices.end()->first+1,  0.0, 400, 300, "gazelle1.bmp" );
//        add_interfaced_vertex(tempoPourMap[0],  0.0, 400, 300, "gazelle1.bmp" );
//    }

}

void Graph::ajouter_Arete()
{
    //std::map<int, Vertex>::iterator it;
    int temp;
    int blindage=0;
    std::vector<int> tempoPourMap; ///Variable temporaire pour le tri

    for ( std::map<int,Vertex>::iterator it= m_vertices.begin(); it != m_vertices.end(); ++it)
    {
        if ((it->second).m_interface->m_top_box.getFromAjout() == true)
        {
            for ( std::map<int,Vertex>::iterator op= m_vertices.begin(); op != m_vertices.end(); ++op)
            {
                if(((op->second).m_interface->m_top_box.getToAjout() == true) && op->first != it->first)
                {

                    for (std::map<int, Edge>::iterator ito= m_edges.begin(); ito != m_edges.end(); ++ito)
                    {

                        tempoPourMap.push_back(ito->first);
                    }
                    for (unsigned int i=0; i<tempoPourMap.size(); ++i)
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

                    for (std::map<int, Edge>::iterator blind= m_edges.begin(); blind != m_edges.end(); ++blind )
                    {
                        if( (blind->second).getFrom() == it->first && (blind->second).getTo() == op->first )
                            blindage=1;
                    }
                    if( blindage != 1)
                    {
                        add_interfaced_edge(tempoPourMap[0]+1, it->first, op->first, 50.0);
                    }
                    (it->second).m_interface->m_top_box.setFromAjout(false);
                    (it->second).m_interface->m_top_box.setToAjout(false);
                    (op->second).m_interface->m_top_box.setToAjout(false);
                    (op->second).m_interface->m_top_box.setFromAjout(false);
                }
            }
        }
    }
}


