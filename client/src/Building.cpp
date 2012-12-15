#include "Building.hpp"

#include <iostream>

using namespace Citizens;

Building::Building(irr::scene::ISceneManager& scmn,irr::io::path af) : sm(scmn)
{
	asset_file = af;
	reload(false);
	mesh_node = sm.addAnimatedMeshSceneNode(
		sm.getMesh(model),
		0,
		-1,
		position,
		orientation,
		scale
	);
	reset();
}

void Building::reload(bool and_reset)
{
	irr::io::IXMLReader* xml = sm.getFileSystem()->createXMLReader(asset_file.c_str());
	if(xml == 0)
	{
		//! \todo rather than segfaulting, load a 'default', known-good asset in place of broken assets
		std::cerr << "Broken building (segfault incoming)" << std::endl;
	}
	
	enum element_name
	{
		ASSET,
		VECTOR_POSITION,
		VECTOR_ORIENTATION,
		VECTOR_SCALE,
		TEXTURE,
		MODEL
	} current_element;
	
	irr::core::stringw ele_vec = L"vector";
	irr::core::stringw ele_tex = L"texture";
	irr::core::stringw ele_mod = L"model";
	irr::core::stringw ele_x = L"x";
	irr::core::stringw ele_y = L"y";
	irr::core::stringw ele_z = L"z";
	irr::core::stringw att_pos = L"position";
	irr::core::stringw att_ori = L"orientation";
	irr::core::stringw att_sca = L"scale";
	
	current_element = ASSET;
	
	while(xml->read())
	{
		switch(xml->getNodeType())
		{
			case irr::io::EXN_ELEMENT:
			{
				if(ele_vec.equals_ignore_case(xml->getNodeName()))
				{
					if(att_pos.equals_ignore_case(xml->getAttributeValueSafe(L"type"))) current_element = VECTOR_POSITION;
					else if(att_ori.equals_ignore_case(xml->getAttributeValueSafe(L"type"))) current_element = VECTOR_ORIENTATION;
					else if(att_sca.equals_ignore_case(xml->getAttributeValueSafe(L"type"))) current_element = VECTOR_SCALE;
				}
				else if(ele_tex.equals_ignore_case(xml->getNodeName()))
				{
					skins[xml->getAttributeValueSafe(L"name")] = sm.getFileSystem()->getFileDir(asset_file.c_str()) + "/" + xml->getAttributeValueSafe(L"path");
					current_element = TEXTURE;
				}
				else if(ele_mod.equals_ignore_case(xml->getNodeName()))
				{
					model = sm.getFileSystem()->getFileDir(asset_file.c_str()) + "/" + xml->getAttributeValueSafe(L"path");
					
					current_element = MODEL;
				}
				
				if(current_element == VECTOR_POSITION || current_element == VECTOR_ORIENTATION || current_element == VECTOR_SCALE)
				{
					if(ele_x.equals_ignore_case(xml->getNodeName()))
					{
						(current_element == VECTOR_POSITION ? position.X : 
						(current_element == VECTOR_ORIENTATION ? orientation.X :
						scale.X)) 
						= xml->getAttributeValueAsFloat(L"value");
					}
					else if(ele_y.equals_ignore_case(xml->getNodeName()))
					{
						(current_element == VECTOR_POSITION ? position.Y : 
						(current_element == VECTOR_ORIENTATION ? orientation.Y :
						scale.Y)) 
						= xml->getAttributeValueAsFloat(L"value");
					}
					else if(ele_z.equals_ignore_case(xml->getNodeName()))
					{
						(current_element == VECTOR_POSITION ? position.Z : 
						(current_element == VECTOR_ORIENTATION ? orientation.Z :
						scale.Z)) 
						= xml->getAttributeValueAsFloat(L"value");
					}
				}
				break;
			}
			case irr::io::EXN_ELEMENT_END:
			{
				switch(current_element)
				{
					case MODEL:
					case TEXTURE:
						current_element = ASSET;
						break;
					case VECTOR_POSITION:
					case VECTOR_ORIENTATION:
					case VECTOR_SCALE:
						if(ele_vec.equals_ignore_case(xml->getNodeName()))
							current_element = ASSET;
						break;
					case ASSET:
						std::cout << "Loaded asset: " << name << std::endl;
						break;
				}
				break;
			}
			/* compiler gag (ignore everything that isn't an element) */
			default:
				break;
		}
	}
	if(and_reset) reset();
}

void Building::reset(void)
{
	mesh_node->setPosition(position);
	mesh_node->setRotation(orientation);
	mesh_node->setScale(scale);
	
	unsigned int i = 0;
	for(std::map<irr::core::stringw,irr::io::path>::iterator skin = skins.begin(); skin != skins.end(); ++skin)
	{
		irr::video::ITexture* texture = 
			sm.getVideoDriver()->getTexture(skin->second);
		mesh_node->setMaterialFlag(irr::video::EMF_LIGHTING,false);
		mesh_node->setMaterialTexture(i,texture);
		mesh_node->setMaterialType(irr::video::EMT_SOLID);
		//! \todo add texture scale and positioning to XML and Building class
		mesh_node->getMaterial(0).getTextureMatrix(i).setTextureScale(1,1);
		i++;
	}
	
	
	
	irr::scene::ITriangleSelector* selector = sm.createTriangleSelector(
		mesh_node
	);
	mesh_node->setTriangleSelector(selector);
	selector->drop();
	
	mesh_node->setMaterialFlag(irr::video::EMF_LIGHTING, true);
    mesh_node->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, true);
	mesh_node->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, true);
}

void Building::unload(void)
{
	mesh_node->remove();
}

irr::scene::IAnimatedMeshSceneNode* Building::get_mesh_node(void)
{
	return mesh_node;
}
