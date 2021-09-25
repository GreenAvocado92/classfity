#ifndef CLASSIFICATE_HPP_
#define CLASSIFICATE_HPP_

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

namespace ZS {
    class Classify {
        public:
        Classify();
        ~Classify();
        /*
        ** \brief raw points with label or make label
        **
        */
        bool set_train_dataset(const std::string &in);
        bool set_train_dataset(const Point_set &in_point) {
            raw_points_with_label_.clear();
            raw_points_with_label_ = in_point;
        };
        /*
        ** @TODO the method of label       
        */
        bool set_train_dataset(const std::vector<std::string> &ins);
        bool set_train_dataset(const std::vector<Point_set> &ins);
        
        /*
        ** \brief the based method to generate features
        **
        */
        bool generate_features(int nb);

        /*
        ** @TODO nulti method to generate features
        **/

        /*
        ** \brief set label
        **
        **/
        bool set_labels();

        /*
        **
        ** 
        */
        
        void train();

        private:
            Point_set raw_points_with_label_;
            Feature_set features_;
            Label_set labels_;
            Imap label_map_;
    }

    bool Classify::set_train_dataset(const std::string &in) {
        raw_points_with_label_.clear();
        std::ifstream infile (in.c_str(), std::ios::binary);
        in >> raw_points_with_label_;

        bool lm_found = false;
        std::tie (label_map_, lm_found) = pts.property_map<int> ("label");
        return true;
    };

    bool Classify::generate_features(int nb = 5) {
        std::size_t number_of_scales = nb;
        Feature_generator generator (raw_points_with_label_,
                                     raw_points_with_label_.point_map(),
                                     number_of_scales);

        features_.begin_parallel_additions();
        generator.generate_point_based_features (features_);
        features_.end_parallel_additions();
    }

    bool Classify::train() {
        Classification::ETHZ::Random_forest_classifier classifier (labels_, features_);
        classifier.train<CGAL::Parallel_if_available_tag> (raw_points_with_label_.range(label_map_),
                                                           800);
    }

} // end of namespace zs


#endif