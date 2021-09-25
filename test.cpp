#define CGAL_LINKED_WITH_TBB

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Classification.h>
#include <CGAL/Point_set_3.h>
#include <CGAL/Point_set_3/IO.h>

#include <CGAL/Real_timer.h>

typedef CGAL::Simple_cartesian<double> Kernel;
typedef Kernel::Point_3 Point;
typedef CGAL::Point_set_3<Point> Point_set;
typedef Kernel::Iso_cuboid_3 Iso_cuboid_3;

typedef Point_set::Point_map Pmap;
typedef Point_set::Property_map<int> Imap;

namespace Classification = CGAL::Classification;

typedef Classification::Label_handle                                            Label_handle;
typedef Classification::Feature_handle                                          Feature_handle;
typedef Classification::Label_set                                               Label_set;
typedef Classification::Feature_set                                             Feature_set;

// typedef Classification::Sum_of_weighted_features_classifier                     Classifier;
typedef Classification::ETHZ::Random_forest_classifier                          Classifier;
typedef Classification::Point_set_feature_generator<Kernel, Point_set, Pmap>    Feature_generator;

int main(int argc, char **argv) {
    std::string filename (argc > 1 ? argv[1] : "data/b9.ply");
    std::ifstream in (filename.c_str(), std::ios::binary);
    Point_set pts;

    std::cerr << "Reading input" << std::endl;
    in >> pts;

    Feature_set features;
    CGAL::Real_timer t;
    t.start();
    std::size_t number_of_scales = 12;
    Feature_generator generator (pts, pts.point_map(), number_of_scales);

    features.begin_parallel_additions();
    generator.generate_point_based_features (features);
    features.end_parallel_additions();

    t.stop();
    std::cerr << "Generate features " << t.time() << " second(s)" << std::endl;

    Label_set labels = { "ground", "vegetation", "roof" };
    Classifier classifier (labels, features);
    std::cerr << "classifier init successful " << std::endl;
    
    std::ifstream incon("config.xml", std::ios_base::in | std::ios_base::binary);
    std::string str = "";
    while (getline(incon, str))
      std::cout << "str = " << str << std::endl;

    classifier.load_configuration(incon);
    incon.close();
    std::cerr << "classifier.load_configuration(incon) " << std::endl;

    // calssificate for all points
    std::vector<int> label_indices(pts.size(), -1);
    Classification::classify_with_graphcut<CGAL::Parallel_if_available_tag>
      (pts, pts.point_map(), labels, classifier,
       generator.neighborhood().k_neighbor_query(12),
       0.2f, 10, label_indices);

    std::cout << "classify_with_graphcut" << std::endl;
    
    // output classificate
    std::ofstream out_a("./a.asc");
    std::ofstream out_b("./b.asc");
    std::ofstream out_c("./c.asc");
    for (int i = 0; i < label_indices.size(); ++i) {
      auto it =  pts.point_map()[i];
      if (label_indices[i] == 0)
        out_a << it.x() << " " <<  it.y() << " " << it.z() << std::endl;
      if (label_indices[i] == 1)
        out_b << it.x() << " " <<  it.y() << " " << it.z() << std::endl;
      if (label_indices[i] == 2)
        out_c << it.x() << " " <<  it.y() << " " << it.z() << std::endl;
    }

    out_a.close();
    out_b.close();
    out_c.close();
    std::cout << "分类完成" << std::endl;
    return 0;
} 