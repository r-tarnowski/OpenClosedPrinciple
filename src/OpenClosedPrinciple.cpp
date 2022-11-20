#include <iostream>
#include <vector>
#include <fstream>
#include <boost/lexical_cast.hpp>


using std::cout;
using std::endl;

void printHeader() {
   cout << endl;
   cout << "================================================" << endl;
   cout << "Design Patterns in Modern C++ by Dmitri Nesteruk" << endl;
   cout << "SOLID, Open Closed Principle" << endl;
   cout << "================================================" << endl;
   cout << endl;
}

enum class Color { Red, Green, Blue };
enum class Size { Small, Medium, Large };

struct Product {

    typedef std::vector< Product * > Items;

    std::string name;
    Color color;
    Size size;

    static void getColorName( Color productColor, std::string & colorName );
    static void getSizeName( Size productSize, std::string & sizeName );
};

struct ProductFilter {
    static Product::Items filterByColor( Product::Items & items, Color color );
    static Product::Items filterBySize( Product::Items & items, Size size );
    static Product::Items filterByColorAndSize( Product::Items & items, Color color, Size size );
};

Product::Items ProductFilter::filterByColor( Product::Items & items, Color color ) {
    Product::Items result;
    for ( auto & item : items ) {
        if ( item->color == color ) {
            result.push_back( item );
        }
    }
    return result;
}

Product::Items ProductFilter::filterBySize( Product::Items & items, Size size ) {
    Product::Items result;
    for ( auto & item : items ) {
        if ( item->size == size ) {
            result.push_back( item );
        }
    }
    return result;
}

Product::Items ProductFilter::filterByColorAndSize( Product::Items & items, Color color, Size size ) {
    Product::Items result;
    for ( auto & item : items ) {
        if ( ( item->size == size ) && ( item->color == color ) ) {
            result.push_back( item );
        }
    }
    return result;
}

//template < typename T > struct AndSpecification;

template <typename T> struct Specification {

    virtual bool is_satisfied( T * item ) = 0;
//    AndSpecification<T> operator &&( Specification<T>&& other ) {
//        return AndSpecification<T>( *this, other );
//    }
};

template < typename T> struct Filter {
    virtual std::vector< T * > filter( std::vector< T * > items, Specification<T> & specification ) = 0;
};

struct BetterFilter : Filter< Product > {
    std::vector< Product * > filter( std::vector< Product * > items,
                                     Specification< Product > & specification ) override {
        std::vector< Product * > result;
        for ( auto & ptrProduct : items ) {
            if ( specification.is_satisfied( ptrProduct ) ) {
                result.push_back( ptrProduct );
            }
        }
        return result;
    }
};

struct ColorSpecification : Specification< Product > {
    Color color;
    explicit ColorSpecification( const Color color )
                               : color( color ) {}
    bool is_satisfied( Product * item ) override {
        return ( item->color == color );
    }
};

struct SizeSpecification : Specification< Product > {
    Size size;
    explicit SizeSpecification( const Size size )
                               : size( size ) {}
    bool is_satisfied( Product * item ) override {
        return ( item->size == size );
    }
};

template < typename T > struct AndSpecification : Specification< T > {
    Specification< T > & first;
    Specification< T > & second;

    AndSpecification( Specification<T> & first, Specification< T > & second )
                    : first( first ),
                      second( second ) {}

    bool is_satisfied( T * item ) override {
        return ( first.is_satisfied( item ) && second.is_satisfied( item ) );
    }

};

void Product::getColorName( Color productColor, std::string & colorName ) {
    switch( productColor ) {
        case Color::Blue:
            colorName = "blue";
            break;

        case Color::Red:
            colorName = "red";
            break;

        case Color::Green:
            colorName = "green";
            break;

        default:
            colorName = "undefined";
            break;
    }
}

void Product::getSizeName( Size productSize, std::string & sizeName ) {
    switch( productSize ) {
        case Size::Small:
            sizeName = "small";
            break;

        case Size::Medium:
            sizeName = "medium";
            break;

        case Size::Large:
            sizeName = "large";
            break;

        default:
            sizeName = "undefined";
            break;
    }
}

void printItems( std::string nameOfItems,  Product::Items & items ) {
    size_t numOfItems = items.size();

    cout << "Number of items in " << nameOfItems <<  " : " << numOfItems << endl;
    if ( numOfItems > 0 ) {
        unsigned int itemIndex = 0;
        std::string itemColor;
        std::string itemSize;
        for ( auto & itemPtr : items ) {
            Product::getColorName( itemPtr->color, itemColor );
            Product::getSizeName( itemPtr->size, itemSize );
            cout << "Item " << ++ itemIndex << " >> name: " << itemPtr->name << ", color: " << itemColor
                 << ", size: " << itemSize << endl;
        }

    }

}

int main(int argc, char *argv[]) {
   printHeader();

   Product::Items allItems;
   Product product1 = { "RedAndSmall", Color::Red, Size::Small };
   allItems.push_back( & product1 );
   Product product2 = { "RedAndMedium", Color::Red, Size::Medium };
   allItems.push_back( & product2 );
   Product product3 = { "RedAndLarge", Color::Red, Size::Large };
   allItems.push_back( & product3 );
   Product product4 = { "GreenAndSmall", Color::Green, Size::Small };
   allItems.push_back( & product4 );
   Product product5 = { "GreenAndMedium", Color::Green, Size::Medium };
   allItems.push_back( & product5 );
   Product product6 = { "GreenAndLarge", Color::Green, Size::Large };
   allItems.push_back( & product6 );
   Product product7 = { "BlueAndSmall", Color::Blue, Size::Small };
   allItems.push_back( & product7 );
   Product product8 = { "BlueAndMedium", Color::Blue, Size::Medium };
   allItems.push_back( & product8 );
   Product product9 = { "BlueAndLarge", Color::Blue, Size::Large };
   allItems.push_back( & product9 );
   printItems( "allItems", allItems );

   cout << "Filtering green items:" << endl;
   Product::Items greenItems = ProductFilter::filterByColor( allItems, Color::Green );
   printItems( "greenItems", greenItems );

   cout << "Filtering medium items:" << endl;
   Product::Items mediumItems = ProductFilter::filterBySize( allItems, Size::Medium );
   printItems( "mediumItems", mediumItems );

   cout << "Filtering green and medium items:" << endl;
   Product::Items greenAndMediumItems = ProductFilter::filterByColorAndSize( allItems, Color::Green, Size::Medium );
   printItems( "greenAndMediumItems", greenAndMediumItems );

   BetterFilter betterFilter;
   cout << "Filtering blue items:" << endl;
   ColorSpecification blueSpecification( Color::Blue );
   auto blueItems = betterFilter.filter( allItems, blueSpecification );
   printItems( "blueItems", blueItems );

   cout << "Filtering large items:" << endl;
   SizeSpecification largeSpecification( Size::Large );
   auto largeItems = betterFilter.filter( allItems, largeSpecification );
   printItems( "largeItems", largeItems );

   cout << "Filtering blue and large items:" << endl;
   AndSpecification< Product > blueAndLargeSpecification( blueSpecification, largeSpecification );
   auto blueAndLargeItems = betterFilter.filter( allItems, blueAndLargeSpecification );
   printItems( "blueAndLargeItems", blueAndLargeItems );

//   cout << "Filtering green and large items:" << endl;
//   auto greenAndLargeSpecification = ColorSpecification( Color::Green ) &&
//                                                       SizeSpecification( Size::Large );
//   auto greenAndLargeItems = betterFilter.filter( allItems, greenAndLargeSpecification);
 //  printItems( "greenAndLargeItems", greenAndLargeItems );

   cout << endl;
   return 0;
}
