#include "draw.h"
#include "model.h"

// this is the drawing  module where most drawing work is done

namespace Software2552 {
	void VideoEngine::draw(Video*v) {
		ofVideoPlayer::draw(v->getStartingPoint().x, v->getStartingPoint().y);
	}
	void BackgroundEngine::setup(Colors* colors) { 
		mode = OF_GRADIENT_LINEAR; 
		setRefreshRate(6000);// just set something different while in dev
	}
	// colors and background change over time but not at the same time
	void BackgroundEngine::update(Colors*colors) {
		if (colors->refresh()) {
			colors->getNextColors();
		}
		//bugbug can add other back grounds like a video loop, sound
		// picture, any graphic etc
		if (refresh()) {
			switch ((int)ofRandom(0, 3)) {
			case 0:
				mode = OF_GRADIENT_LINEAR;
				break;
			case 1:
				mode = OF_GRADIENT_CIRCULAR;
				break;
			case 2:
				mode = OF_GRADIENT_BAR;
				break;
			}
		}

	}

	void BackgroundEngine::draw(Colors* colors){
		ofBackgroundGradient(ofColor::fromHex(colors->getCurrentColors().getForeground()),
			ofColor::fromHex(colors->getCurrentColors().getBackground()), mode);
		TextEngine::draw("print", 100,200);
	}
	void TextEngine::draw(const string &s, int x, int y) {
		ofPushStyle();
		ofSetHexColor(Colors::getCurrentColors().getFontColor());
		Font font;
		font.get().drawString(s, x, y);
		ofPopStyle();
	}
	void TextEngine::draw(Text* t) {
		ofPushStyle();
		ofSetHexColor(Colors::getCurrentColors().getFontColor());
		t->getFont().drawString(t->getText(), t->getStartingPoint().x, t->getStartingPoint().y);
		ofPopStyle();
	}

	ofxBox2dParticleSystem::ofxBox2dParticleSystem() {
		flag = b2_waterParticle;
		useTexture = false; // added
	}

	void ofxBox2dParticleSystem::setup(b2World * _b2world) {
		ofColor color;
		color.set(127, 200, 255);
		setup(_b2world, 10000, 0.0, 6.0, 4.0, color);
	}

	void ofxBox2dParticleSystem::setup(b2World * _b2world, int maxCount) {
		ofColor color;
		color.set(127, 200, 255);
		setup(_b2world, maxCount, 0.0, 6.0, 4.0, color);
	}

	void ofxBox2dParticleSystem::setup(b2World * _b2world, int _maxCount, float _lifetime, float _radius, float _particleSize, ofColor _color) {
		b2dworld = _b2world;
		particleSystemDef.radius = _radius / OFX_BOX2D_SCALE;
		particleSystemDef.maxCount = _maxCount;
		lifetime = _lifetime;
		particleSize = _particleSize;
		color = _color;

		particleSystem = b2dworld->CreateParticleSystem(&particleSystemDef);
	}

	void ofxBox2dParticleSystem::draw() {
		ofFill();

		int32 particleCount = particleSystem->GetParticleCount();
		b2Vec2 *positnon = particleSystem->GetPositionBuffer();
		b2ParticleColor *particleColor = particleSystem->GetColorBuffer();

		ofVboMesh mesh;
		mesh.setMode(OF_PRIMITIVE_POINTS);

		for (int i = 0; i < particleCount; i++) {
			mesh.addVertex(ofVec2f(positnon[i].x, positnon[i].y));
			mesh.addColor(ofFloatColor(particleColor[i].r / 255.0, particleColor[i].g / 255.0, particleColor[i].b / 255.0));
		}

		ofSetColor(255);
		ofEnablePointSprites();
		ofPushMatrix();
		ofScale(OFX_BOX2D_SCALE, OFX_BOX2D_SCALE);
		glPointSize(particleSize);
		if (useTexture) {
			textureImage.getTextureReference().bind();
			mesh.drawFaces();
			textureImage.getTextureReference().unbind();
		}
		else {
			mesh.draw();
		}
		ofPopMatrix();
		ofDisablePointSprites();
	}

	int32 ofxBox2dParticleSystem::createParticle(ofVec2f position, ofVec2f velocity) {
		return createParticle(position.x, position.y, velocity.x, velocity.y);
	}

	int32 ofxBox2dParticleSystem::createParticle(float posx, float posy, float velx, float vely) {
		b2ParticleDef pd;
		pd.flags = flag;
		b2Vec2 position = b2Vec2(posx / OFX_BOX2D_SCALE, posy / OFX_BOX2D_SCALE);
		pd.position = position;
		pd.velocity.Set(velx / OFX_BOX2D_SCALE, vely / OFX_BOX2D_SCALE);
		pd.color = b2ParticleColor(color.r, color.g, color.b, color.a);
		if (lifetime > 0.0) {
			pd.lifetime = lifetime;
		}
		return particleSystem->CreateParticle(pd);
	}

	void ofxBox2dParticleSystem::applyForce(int32 particle_index, const ofVec2f& force)
	{
		applyForce(particle_index, force.x, force.y);
	}

	void ofxBox2dParticleSystem::applyForce(int32 particle_index, float force_x, float force_y)
	{
		particleSystem->ParticleApplyForce(particle_index, b2Vec2(force_x, force_y));
	}

	void ofxBox2dParticleSystem::createRectParticleGroup(ofVec2f position, ofVec2f size, ofColor color) {
		b2PolygonShape rect;
		rect.SetAsBox(size.x / OFX_BOX2D_SCALE, size.y / OFX_BOX2D_SCALE);

		b2ParticleGroupDef pgd;
		pgd.flags = flag;
		pgd.position = b2Vec2(position.x / OFX_BOX2D_SCALE, position.y / OFX_BOX2D_SCALE);
		pgd.shape = &rect;
		pgd.color = b2ParticleColor(color.r, color.g, color.b, color.a);
		if (lifetime > 0.0) {
			pgd.lifetime = lifetime;
		}
		particleSystem->CreateParticleGroup(pgd);
	}

	void ofxBox2dParticleSystem::createCircleParticleGroup(ofVec2f position, float radius, ofColor color) {
		b2CircleShape circle;
		circle.m_radius = radius / OFX_BOX2D_SCALE;

		b2ParticleGroupDef pgd;
		pgd.flags = flag;
		pgd.position = b2Vec2(position.x / OFX_BOX2D_SCALE, position.y / OFX_BOX2D_SCALE);
		pgd.shape = &circle;
		pgd.color = b2ParticleColor(color.r, color.g, color.b, color.a);
		if (lifetime > 0.0) {
			pgd.lifetime = lifetime;
		}
		particleSystem->CreateParticleGroup(pgd);
	}

	void ofxBox2dParticleSystem::loadImage(string filename) {
		ofDisableArbTex();
		textureImage.loadImage(filename);
		useTexture = true;
	}

	//--------------------------------------------------------------
	void ofxBox2dParticleSystem::setRadius(float radius) {
		particleSystemDef.radius = radius / OFX_BOX2D_SCALE;
	}

	void ofxBox2dParticleSystem::setParticleLifetime(float _lifetime) {
		lifetime = _lifetime;
	}

	void ofxBox2dParticleSystem::setColor(ofColor _color) {
		color = _color;
	}

	void ofxBox2dParticleSystem::setParticleFlag(b2ParticleFlag _flag) {
		flag = _flag;
	}

	int ofxBox2dParticleSystem::getParticleCount() {
		return particleSystem->GetParticleCount();
	}
	void ParticlesEngine::draw(Particles*particles) {
		ofPushStyle();
		//ofEnableBlendMode(OF_BLENDMODE_ADD);
		part.draw();
		//ofEnableBlendMode(OF_BLENDMODE_ALPHA);
		ofPopStyle();
		//particles->getFont().drawStringAsShapes("Known years native people were in MN before first treaty (12,000)", 1900, 480);
	}
	void ParticlesEngine::update(Particles*particles) {
		years += 10;
		if (years < 12000) {
			for (int i = 0; i < 10; i++) {
				float radius = ofRandom(160, 280);
				ofVec2f position = ofVec2f(cos(ofRandom(PI * 2.0)), sin(ofRandom(PI * 2.0)));
				ofVec2f velocity = ofVec2f(0.0f, -50.0f);
				ofColor color;
				int hue = int(ofGetFrameNum() / 4.0) % 255;
				color.setHsb(hue, 180, 255);
				part.setColor(color);
				part.setParticleFlag(b2_elasticParticle);
				part.createParticle(position, velocity);
				if (years < 12000) {
					years += 10;
				}
			}


		}
		box2d.update();
	}
	void ParticlesEngine::setup(Particles*particles) {
		box2d.init();
		box2d.setGravity(0, 0);
		box2d.setFPS(30.0);
		box2d.createBounds();
		years = 0;
		ofColor color;
		color.set(255);
		part.setParticleFlag(b2_tensileParticle);
		part.loadImage(particles->getLocation());
		part.setup(box2d.getWorld(), 12000, 160.0, 16.0, 92.0, ofColor(0, 0, ofRandom(128, 255)));

	}
	// its ok also if Controller passes in an object such as a paragraph to copy in
	// bugbug move to a text object
	void Graphics2552::rotateToNormal(ofVec3f normal) {
		normal.normalize();

		float rotationAmount;
		ofVec3f rotationAngle;
		ofQuaternion rotation;

		ofVec3f axis(0, 0, 1);
		rotation.makeRotate(axis, normal);
		rotation.getRotate(rotationAmount, rotationAngle);
		logVerbose("ofRotate " + ofToString(rotationAmount));
		ofRotate(rotationAmount, rotationAngle.x, rotationAngle.y, rotationAngle.z);
	}

}
