#include "WorldPoint.h"
#include "WorldRect.h"
#include "NodeImp.h"
#include "ElementImp.h"
#include "PropertyImp.h"
#include "PropertyTypeList.h"

#include <iostream>
using namespace std;

#define CheckStatement(s) !(s) ? cerr << "failed: "#s << endl, true : false

inline ostream& 
operator<<(ostream& s, const WorldPoint& p)
{
	ios_base::fmtflags old = s.flags();
	s.unsetf(ios_base::floatfield);
	s << '(' << p.x() << ',' << p.y() << ')';
//	fprintf(f, "(%.1f,%.1f)", p.x(), p.y());
	s.setf(old);
	return s;
}

inline ostream& 
operator<<(ostream& s, const WorldRect& r)
{
	ios_base::fmtflags old = s.flags();
	s.unsetf(ios_base::floatfield);
	s << '(' << r.bottomLeft() << ',' << r.topRight() << ')';
//	fprintf(f, "(%.1f,%.1f)", p.x(), p.y());
	s.setf(old);
	return s;
}

inline ostream&
operator<<(ostream& s, const NodeImp& n)
{
	s << "coords:" << n.GetCoords() << ", ";
	s << "restraint:(" << n.GetRestrained(0) << ',' << n.GetRestrained(1) << ',' << n.GetRestrained(2) << "), "; 
//	s << "settlement:(" << n.GetSettlement(0) << ',' << n.GetSettlement(1) << ',' << n.GetSettlement(2) << "), "; 
	s << "equations:(" << n.GetEquationNumber(0) << ',' << n.GetEquationNumber(1) << ',' << n.GetEquationNumber(2) << ")"; 
	return s;
}

inline ostream& 
operator<<(ostream& s, const PropertyImp& p)
{
	const PropertyTypeList* pList = p.GetPropertyTypes();
	pList->Reset();
	DlInt32 count = 0;
	Property prop(const_cast<PropertyImp*>(&p));
	while(pList->HasMore()) {
		const char* n = pList->Next()->name;
		if (count++)
			s << ", ";
		s << n << ": " << prop.GetValue(n, true).get();
	}
	
	return s;
}

inline ostream&
operator<<(ostream& s, const ElementImp& e)
{
	s << "Node 0:" << *e.StartNode() << endl;
	s << "Node 1:" << *e.EndNode() << endl;
	s << "Length:" << e.Length() << endl;
	s << "Properties:" << *e.GetProperty() << endl;
	return s;
}


const double kTolerance = 1.0e-15;
 
