
#include "PotreeReader.h"

const PointAttribute PointAttribute::POSITION_CARTESIAN		= PointAttribute(0, "POSITION_CARTESIAN",	3, 12);
const PointAttribute PointAttribute::COLOR_PACKED			= PointAttribute(1, "COLOR_PACKED",			4, 4);
const PointAttribute PointAttribute::INTENSITY				= PointAttribute(2, "INTENSITY",			1, 2);
const PointAttribute PointAttribute::CLASSIFICATION			= PointAttribute(3, "CLASSIFICATION",		1, 1);
const PointAttribute PointAttribute::NORMAL_SPHEREMAPPED	= PointAttribute(4, "NORMAL_SPHEREMAPPED",	2, 2);
const PointAttribute PointAttribute::NORMAL_OCT16			= PointAttribute(5, "NORMAL_OCT16",			2, 2);
const PointAttribute PointAttribute::NORMAL					= PointAttribute(6, "NORMAL",				3, 12);

PointAttribute PointAttribute::fromString(string name){
	if(name == "POSITION_CARTESIAN"){
		return PointAttribute::POSITION_CARTESIAN;
	}else if(name == "COLOR_PACKED"){
		return PointAttribute::COLOR_PACKED;
	}else if(name == "INTENSITY"){
		return PointAttribute::INTENSITY;
	}else if(name == "CLASSIFICATION"){
		return PointAttribute::CLASSIFICATION;
	}else if(name == "NORMAL_SPHEREMAPPED"){
		return PointAttribute::NORMAL_SPHEREMAPPED;
	}else if(name == "NORMAL_OCT16"){
		return PointAttribute::NORMAL_OCT16;
	}else if(name == "NORMAL"){
		return PointAttribute::NORMAL;
	}

	throw "Invalid PointAttribute name: '" + name + "'";
}

bool operator==(const PointAttribute& lhs, const PointAttribute& rhs){ 
	return lhs.ordinal == rhs.ordinal;
}

vector<Point> PRNode::points(){
	if(!loaded){
		reader->load(this);
	}

	return _points;
}




void PotreeReader::load(PRNode *node){
	
	string fNode = file + "/../data/" + getHierarchyPath(node) + "/" + node->name + ".bin";

	vector<char> buffer;
	{
		ifstream in(fNode, std::ios::binary);
		int byteSize = fs::file_size(fNode);

		buffer.resize(byteSize, 0);
		in.read(reinterpret_cast<char*>(&buffer[0]), byteSize);
	}

	vector<Point> points;
	points.resize(node->numPoints);

	glm::dvec3 &scale = metadata.scale;
	glm::dvec3 &min = metadata.boundingBox.min;
	int offset = 0;
	for(int i = 0; i < node->numPoints; i++){

		Point &point = points[i];
		
		for(auto attribute : metadata.pointAttributes.attributes){

			if(attribute == PointAttribute::POSITION_CARTESIAN){
				auto coordinates = reinterpret_cast<unsigned int*>(&buffer[offset]);

				unsigned int ux = coordinates[0];
				unsigned int uy = coordinates[1];
				unsigned int uz = coordinates[2];

				double x = ux * scale.x + min.x;
				double y = uy * scale.y + min.y;
				double z = uz * scale.z + min.z;

				point.position.x = x;
				point.position.y = y;
				point.position.z = z;

				offset += 12;
			}else if(attribute == PointAttribute::COLOR_PACKED){
				auto colors = reinterpret_cast<unsigned char*>(&buffer[offset]);

				point.color.r = colors[0];
				point.color.g = colors[1];
				point.color.b = colors[2];
			}

		}
	}

	node->_points = points;
	node->loaded = true;
}











