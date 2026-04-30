#include "vector_map_app/map_data.hpp"
#include "vector_map_app/map.hpp"

#include <fstream>

std::ostream& operator<<(std::ostream& out, QPolygonF& obj)
{
  for (const QPointF& p : obj)
    out << "(" << std::to_string(p.x()) << "," << std::to_string(p.y()) << ")" << " ";

  return out;
}

void MapData::saveTo(const QString& filename, Map* map, QTransform& map_offset, float resolution)
{
  std::ofstream file(filename.toStdString());

  if (!file.is_open())
    throw std::runtime_error("Can't open file to save roads!");

  file << "resolution: " << std::to_string(resolution) << std::endl;

  std::map<Vertice, std::vector<Vertice>> graph = map->getGraph();

  file << "Graph:" << std::endl;
  for(const auto& [vertice, adj_vertices] : graph)
  {
    file << std::to_string(vertice.id) << "(" << std::to_string(vertice.coord.x()) << "," << std::to_string(vertice.coord.y()) << ")" << "-";
    
    for(const auto& v : adj_vertices)
      file << std::to_string(v.id) << "(" << std::to_string(vertice.coord.x()) << "," << std::to_string(vertice.coord.y()) << ")" << " ";

    file << std::endl;
  }

  file << std::endl;

  std::vector<std::vector<std::tuple<Vertice, Vertice, QPolygonF, int>>> polygons = map->getGraphPolygons();

  size_t polygons_count = polygons.size();

  size_t poly_idx = 0;
  for(size_t i = 0; i < polygons_count; ++i)
  {
    for(size_t j = 0; j < polygons[i].size(); ++j)
    {
      QPolygonF poly;
      Vertice lhs, rhs;
      int type;
      std::tie(lhs, rhs, poly, type) = polygons[i][j];

      lhs.coord = map_offset.map(lhs.coord);
      rhs.coord = map_offset.map(rhs.coord);

      scaleVertice(lhs, resolution);
      scaleVertice(rhs, resolution);

      scalePointRange(poly.begin(), poly.end(), resolution);

      file << "Polygon-" << std::to_string(i) << "-" 
                         << std::to_string(lhs.id) << "(" << lhs.coord.x() << "," << lhs.coord.y() << ")" << "-"
                         << std::to_string(rhs.id) << "(" << rhs.coord.x() << "," << rhs.coord.y() << ")" << "-"
                         << std::to_string(type)   << ":" << " " << poly << std::endl;
    }
  }
}

void MapData::scaleVertice(Vertice& v, float resolution)
{
  v.coord.setX(v.coord.x() * resolution);
  v.coord.setY(v.coord.y() * resolution);
}

void MapData::loadTo(const QString& filename, Map* map)
{
  std::ifstream file(filename.toStdString());

  if (!file.is_open())
    throw std::runtime_error("Can't open file to load roads!");

  std::string line;

  bool graph_process = true;

  std::map<Vertice, std::vector<Vertice>> graph;
  std::vector<std::vector<std::tuple<Vertice, Vertice, QPolygonF, int>>> polygons;
  int max_id = -1;

  Vertice main;

  float resolution;

  std::getline(file, line);

  if (line.size() == 0)
    throw std::runtime_error("No resolution field found!");

  size_t d = line.find(" ");
  std::string r = line.substr(++d, line.size());

  resolution = std::stof(r);

  while (std::getline(file, line))
  {
    if (line.size() == 0)
    {
      graph_process = false;
      continue;
    }
    
    size_t delim_pos = line.find(":");
    if (delim_pos != std::string::npos)
    {
      // we found start phrase (Graph: or Polygon-poly_id-graph_id-graph_id:)
      if (graph_process)
      {
        std::string delim_pos_rhs = line.substr(++delim_pos, line.size());

        bool first_glance = true;
        while(delim_pos != std::string::npos)
        {
          delim_pos = delim_pos_rhs.find("(");
          std::string id = delim_pos_rhs.substr(delim_pos);
          delim_pos_rhs = delim_pos_rhs.substr(++delim_pos, delim_pos_rhs.size());

          delim_pos = delim_pos_rhs.find(",");
          std::string coord_x = delim_pos_rhs.substr(delim_pos);
          delim_pos_rhs = delim_pos_rhs.substr(++delim_pos, delim_pos_rhs.size());

          delim_pos = delim_pos_rhs.find(")");
          std::string coord_y = delim_pos_rhs.substr(delim_pos);
          delim_pos_rhs = delim_pos_rhs.substr(++delim_pos, delim_pos_rhs.size());
          
          if (first_glance)
          {
            delim_pos = delim_pos_rhs.find("-");
            delim_pos_rhs = delim_pos_rhs.substr(++delim_pos, delim_pos_rhs.size());

            // TODO: add here try catch
            QPointF coord(std::stod(coord_x), std::stod(coord_y));
            main.id = std::stoi(id);
            main.coord = coord;
            graph[main] = std::vector<Vertice>();
          }
          else
          {
            delim_pos = delim_pos_rhs.find(" ");
            delim_pos_rhs = delim_pos_rhs.substr(++delim_pos, delim_pos_rhs.size());

            QPointF coord(std::stod(coord_x), std::stod(coord_y));
            Vertice temp {std::stoi(id), coord};

            graph[main].push_back(temp);
          }
        }
      }
      else
      {
        std::string delim_pos_lhs = line.substr(delim_pos);

        size_t delim_pos_slave = delim_pos_lhs.find("-");
        delim_pos_lhs = delim_pos_lhs.substr(++delim_pos_slave, delim_pos_lhs.size());

        delim_pos_slave = delim_pos_lhs.find("-");
        std::string main_id_s = delim_pos_lhs.substr(delim_pos_slave);
        delim_pos_lhs = delim_pos_lhs.substr(++delim_pos_slave, delim_pos_lhs.size());

        delim_pos_slave = delim_pos_lhs.find("(");
        std::string lhs_id_s = delim_pos_lhs.substr(delim_pos_slave);
        delim_pos_lhs = delim_pos_lhs.substr(++delim_pos_slave, delim_pos_lhs.size());

        delim_pos_slave = delim_pos_lhs.find(",");
        std::string lhs_coord_x_s = delim_pos_lhs.substr(delim_pos_slave);
        delim_pos_lhs = delim_pos_lhs.substr(++delim_pos_slave, delim_pos_lhs.size());

        delim_pos_slave = delim_pos_lhs.find(")");
        std::string lhs_coord_y_s = delim_pos_lhs.substr(delim_pos_slave);
        delim_pos_lhs = delim_pos_lhs.substr(++delim_pos_slave, delim_pos_lhs.size());

        delim_pos_slave = delim_pos_lhs.find("-");
        delim_pos_lhs = delim_pos_lhs.substr(++delim_pos_slave, delim_pos_lhs.size());

        delim_pos_slave = delim_pos_lhs.find("(");
        std::string rhs_id_s = delim_pos_lhs.substr(delim_pos_slave);
        delim_pos_lhs = delim_pos_lhs.substr(++delim_pos_slave, delim_pos_lhs.size());

        delim_pos_slave = delim_pos_lhs.find(",");
        std::string rhs_coord_x_s = delim_pos_lhs.substr(delim_pos_slave);
        delim_pos_lhs = delim_pos_lhs.substr(++delim_pos_slave, delim_pos_lhs.size());

        delim_pos_slave = delim_pos_lhs.find(")");
        std::string rhs_coord_y_s = delim_pos_lhs.substr(delim_pos_slave);
        delim_pos_lhs = delim_pos_lhs.substr(++delim_pos_slave, delim_pos_lhs.size());

        delim_pos_slave = delim_pos_lhs.find("-");
        delim_pos_lhs = delim_pos_lhs.substr(++delim_pos_slave, delim_pos_lhs.size());

        std::string type_s = delim_pos_lhs;

        std::string delim_pos_rhs = line.substr(++delim_pos);

        QPolygonF poly;

        while(delim_pos != std::string::npos)
        {
          delim_pos = delim_pos_rhs.find("(");
          delim_pos_rhs = delim_pos_rhs.substr(++delim_pos);

          delim_pos = delim_pos_rhs.find(",");
          std::string coord_x = delim_pos_rhs.substr(delim_pos);
          delim_pos_rhs = delim_pos_rhs.substr(++delim_pos, delim_pos_rhs.size());

          delim_pos = delim_pos_rhs.find(")");
          std::string coord_y = delim_pos_rhs.substr(delim_pos);
          delim_pos_rhs = delim_pos_rhs.substr(++delim_pos, delim_pos_rhs.size());

          QPointF coord(std::stod(coord_x), std::stod(coord_y));

          poly.push_back(coord);
        }

        int main_id = std::stoi(main_id_s);
        if (main_id > max_id)
        {
          polygons.resize(main_id);
          max_id = main_id;
        }

        int lhs_id = std::stoi(lhs_id_s);
        QPointF lhs_coord(std::stod(lhs_coord_x_s), std::stod(lhs_coord_y_s));
        Vertice lhs {lhs_id, lhs_coord};
        int rhs_id = std::stoi(rhs_id_s);
        QPointF rhs_coord(std::stod(rhs_coord_x_s), std::stod(rhs_coord_y_s));
        Vertice rhs {rhs_id, rhs_coord};
        int type = std::stoi(type_s);

        polygons[main_id].push_back(std::tie(lhs, rhs, poly, type));
      }
    }
    else
    {
      std::string delim_pos_rhs = line;

      bool first_glance = true;
      while(delim_pos != std::string::npos)
      {
        delim_pos = delim_pos_rhs.find("(");
        std::string id = delim_pos_rhs.substr(delim_pos);
        delim_pos_rhs = delim_pos_rhs.substr(++delim_pos, delim_pos_rhs.size());

        delim_pos = delim_pos_rhs.find(",");
        std::string coord_x = delim_pos_rhs.substr(delim_pos);
        delim_pos_rhs = delim_pos_rhs.substr(++delim_pos, delim_pos_rhs.size());

        delim_pos = delim_pos_rhs.find(")");
        std::string coord_y = delim_pos_rhs.substr(delim_pos);
        delim_pos_rhs = delim_pos_rhs.substr(++delim_pos, delim_pos_rhs.size());
        
        if (first_glance)
        {
          delim_pos = delim_pos_rhs.find("-");
          delim_pos_rhs = delim_pos_rhs.substr(++delim_pos, delim_pos_rhs.size());

          // TODO: add here try catch
          QPointF coord(std::stod(coord_x), std::stod(coord_y));
          main.id = std::stoi(id);
          main.coord = coord;
          graph[main] = std::vector<Vertice>();
        }
        else
        {
          delim_pos = delim_pos_rhs.find(" ");
          delim_pos_rhs = delim_pos_rhs.substr(++delim_pos, delim_pos_rhs.size());

          QPointF coord(std::stod(coord_x), std::stod(coord_y));
          Vertice temp {std::stoi(id), coord};

          graph[main].push_back(temp);
        }
      }
    } 
  }

  map->loadRoads(polygons);
  map->loadGraph(graph);
}