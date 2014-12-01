#include "OPGpuNoiseDataSource.h"

#include "boost/lexical_cast.hpp"

namespace OgrePlanet
{
	Ogre::Real GpuNoiseDataSource::getValue(const Ogre::Vector3 &position) {
		return 0;
	}

	boost::shared_array<Ogre::Real> GpuNoiseDataSource::getValues(int quads, int padding, const Ogre::Vector3 & min, const Ogre::Vector3 & max)
	{
		Ogre::Timer timer;
		timer.reset();

		Ogre::TexturePtr rtt = Ogre::TextureManager::getSingleton().createManual(
			"rtt", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Ogre::TEX_TYPE_2D, quads + 2*padding + 1, quads + 2*padding + 1, 1, Ogre::PF_FLOAT32_R, Ogre::TU_RENDERTARGET);
		Ogre::RenderTexture * rt = rtt->getBuffer()->getRenderTarget();
		rt->setAutoUpdated(false);

		Ogre::SceneManager * sm = Ogre::Root::getSingleton().createSceneManager("DefaultSceneManager");
		Ogre::Camera * cam = sm->createCamera("RttCamera");
		Ogre::Viewport * vp = rt->addViewport(cam);

		Ogre::CompositorInstance * comp = Ogre::CompositorManager::getSingleton().addCompositor(vp, "OgrePlanet/NoiseTestCompositor");
		Ogre::CompositorManager::getSingleton().setCompositorEnabled(vp, "OgrePlanet/NoiseTestCompositor", true);
		
		CompositorListener listener(quads, padding, min, max);
		comp->addListener(&listener);

		rt->update();

		comp->removeListener(&listener);

		rt->removeAllViewports();
		Ogre::Root::getSingleton().destroySceneManager(sm);

		float * d = new float[rt->getWidth() * rt->getHeight()];
		Ogre::PixelBox pixelBox(rt->getWidth(), rt->getHeight(), 1, Ogre::PF_FLOAT32_R, d);
		rt->copyContentsToMemory(pixelBox, Ogre::RenderTarget::FB_AUTO);

		boost::shared_array<Ogre::Real> data(new Ogre::Real[rt->getWidth() * rt->getHeight()]);
		for (int y = 0; y < rt->getHeight(); y++)
		{
			for (int x = 0; x < rt->getWidth(); x++)
			{
				data[y*rt->getWidth() + x] = d[y*pixelBox.rowPitch + x];
			}
		}

		delete [] d;
		Ogre::TextureManager::getSingleton().remove(rtt->getName());

		unsigned long time = timer.getMicroseconds();

		// rt->writeContentsToFile("rtt.bmp");
		Ogre::LogManager::getSingleton().logMessage("Generating image took " + boost::lexical_cast<Ogre::String, unsigned long>(time) + " microseconds.");

		return data;
	}

	GpuNoiseDataSource::CompositorListener::CompositorListener(int quads, int padding, const Ogre::Vector3 & min, const Ogre::Vector3 & max)
		: mQuads(quads),
		mPadding(padding),
		mMin(min),
		mMax(max)
	{
	}

	void GpuNoiseDataSource::CompositorListener::notifyMaterialSetup(Ogre::uint32 pass_id, Ogre::MaterialPtr & material)
	{
		material->load();

		Ogre::Real sideLength = Ogre::Math::Abs(mMax.x-mMin.x);
		Ogre::Real paddingLength = mPadding * (sideLength / mQuads);

		int face = 6;

		if (mMin.x == 1 && mMax.x == 1)
		{
			// Right face
			mMin.y += paddingLength;
			mMin.z += paddingLength;
			mMax.y -= paddingLength;
			mMax.z -= paddingLength;
			face = 0;
		}
		else if (mMin.x == -1 && mMax.x == -1)
		{
			// Left face
			mMin.y += paddingLength;
			mMin.z -= paddingLength;
			mMax.y -= paddingLength;
			mMax.z += paddingLength;
			face = 1;
		}
		else if (mMin.y == 1 && mMax.y == 1)
		{
			// Top face
			mMin.x -= paddingLength;
			mMin.z -= paddingLength;
			mMax.x += paddingLength;
			mMax.z += paddingLength;
			face = 2;
		}
		else if (mMin.y == -1 && mMax.y == -1)
		{
			// Bottom face
			mMin.x -= paddingLength;
			mMin.z -= paddingLength;
			mMax.x += paddingLength;
			mMax.z += paddingLength;
			face = 3;
		}
		else if (mMin.z == 1 && mMax.z == 1)
		{
			// Front face
			mMin.x -= paddingLength;
			mMin.y += paddingLength;
			mMax.x += paddingLength;
			mMax.y -= paddingLength;
			face = 4;
		}
		else if (mMin.z == -1 && mMax.z == -1)
		{
			// Back face
			mMin.x += paddingLength;
			mMin.y -= paddingLength;
			mMax.x -= paddingLength;
			mMax.y += paddingLength;
			face = 5;
		}

		Ogre::GpuProgramParametersSharedPtr vparams =
			material->getBestTechnique()->getPass(0)->getVertexProgramParameters();

		vparams->setNamedConstant("quads", mQuads + 2*mPadding + 1);

		Ogre::GpuProgramParametersSharedPtr fparams =
			material->getBestTechnique()->getPass(0)->getFragmentProgramParameters();

		//fparams->setNamedConstant("octaves", 8);
		fparams->setNamedConstant("face", face);
		//fparams->setNamedConstant("min", mMin);
		//fparams->setNamedConstant("max", mMax);
	}
}
