#pragma once
extern Camera camera;
extern double currentTimeInSeconds;
extern ID3D11ShaderResourceView* textureView;
extern ID3D11SamplerState* samplerState;
extern double currentTimeInSeconds;

class Scene {
protected:
	// Create Vertex and Index Buffer
	VertexBuffer* cubeVertexBuffer;
	IndexBuffer* cubeIndexBuffer;

	VertexBuffer* planeVertexBuffer;
	IndexBuffer* planeIndexBuffer;

	VertexBuffer* cylinderVertexBuffer;
	IndexBuffer* cylinderIndexBuffer;

	VertexBuffer* sphereVertexBuffer;
	IndexBuffer*  sphereIndexBuffer;

	Shader* lightShader;
	Shader* blinnPhongShader;

	BlinnPhongPSConstants psConstants;
	BlinnPhongVSConstants psConstants;

public:

	virtual void updateLights(Shader* shader, Vector4 dirLight, Vector4 color)
	{
		
		psConstants.dirLight.dirEye = normalise(Vector4{ 1.f, 0.f, 0.f, 0.f });
		psConstants.dirLight.color = { 1.0f, 1.0f, 1.0f, 1.f };
		psConstants.objectColor = { 0.7f, 0.7f, 0.7f , 1.0f };
		psConstants.useColor = true;
		blinnPhongShader->SetPixelShaderUniformBuffer("pbPixelConstants", psConstants);
	}
	virtual void init()
	{
		LoadedObj obj = loadObj("cube.obj");
		cubeVertexBuffer = new VertexBuffer(obj.vertexBuffer, obj.numVertices, sizeof(VertexData), 0, false);
		cubeIndexBuffer = new IndexBuffer(obj.indexBuffer, obj.numIndices);
		freeLoadedObj(obj);

		 obj = loadObj("plane.obj");
		planeVertexBuffer = new VertexBuffer(obj.vertexBuffer, obj.numVertices, sizeof(VertexData), 0, false);
		planeIndexBuffer = new IndexBuffer(obj.indexBuffer, obj.numIndices);
		freeLoadedObj(obj);

		 obj = loadObj("cylinder.obj");
		cylinderVertexBuffer = new VertexBuffer(obj.vertexBuffer, obj.numVertices, sizeof(VertexData), 0, false);
		cylinderIndexBuffer = new IndexBuffer(obj.indexBuffer, obj.numIndices);
		freeLoadedObj(obj);

		 obj = loadObj("sphere.obj");
		sphereVertexBuffer = new VertexBuffer(obj.vertexBuffer, obj.numVertices, sizeof(VertexData), 0, false);
		sphereIndexBuffer = new IndexBuffer(obj.indexBuffer, obj.numIndices);
		freeLoadedObj(obj);


		lightShader = new Shader(L"Lights.hlsl", ShaderInputType::POSITION);

		//e Vertex Shader for rendering our lit objects
		blinnPhongShader = new Shader(L"BlinnPhong.hlsl", ShaderInputType::POSITION_TEXTURE_NORMAL);

		std::pair<std::string, ConstantBuffer> cbp;

		ConstantBuffer blinnPhongVertextConstantBuffer = ConstantBuffer::CreateConstantBuffer<BlinnPhongVSConstants>(0);
		cbp.first = "pbVertexConstants";
		cbp.second = blinnPhongVertextConstantBuffer;
		blinnPhongShader->uniformBuffers.insert(cbp);


		ConstantBuffer blinnPhongPSConstantBuffer = ConstantBuffer::CreateConstantBuffer<BlinnPhongPSConstants>(0);
		cbp.first = "pbPixelConstants";
		cbp.second = blinnPhongPSConstantBuffer;
		blinnPhongShader->uniformBuffers.insert(cbp);
	}
	virtual void update(float dt) = 0;

	void moveCamera(float dt)
	{
		// Update camera
		{
			Vector3 camFwdXZ = normalise(Vector3{ camera.cameraFwd.x, 0, camera.cameraFwd.z });
			Vector3 cameraRightXZ = cross(camFwdXZ, { 0, 1, 0 });

			const float CAM_MOVE_SPEED = 5.f; // in metres per second
			const float CAM_MOVE_AMOUNT = CAM_MOVE_SPEED * dt;
			if (keys[KEY_W])
				camera.cameraPos += camFwdXZ * CAM_MOVE_AMOUNT;
			if (keys[KEY_S])
				camera.cameraPos -= camFwdXZ * CAM_MOVE_AMOUNT;
			if (keys[KEY_A])
				camera.cameraPos -= cameraRightXZ * CAM_MOVE_AMOUNT;
			if (keys[KEY_D])
				camera.cameraPos += cameraRightXZ * CAM_MOVE_AMOUNT;
			if (keys[KEY_Q])
				camera.cameraPos.y += CAM_MOVE_AMOUNT;
			if (keys[KEY_E])
				camera.cameraPos.y -= CAM_MOVE_AMOUNT;

			const float CAM_TURN_SPEED = M_PI; // in radians per second
			const float CAM_TURN_AMOUNT = CAM_TURN_SPEED * dt;
			if (keys[VK_RIGHT])
				camera.cameraYaw -= CAM_TURN_AMOUNT;
			if (keys[VK_LEFT])
				camera.cameraYaw += CAM_TURN_AMOUNT;
			if (keys[VK_UP])
				camera.cameraPitch += CAM_TURN_AMOUNT;
			if (keys[VK_DOWN])
				camera.cameraPitch -= CAM_TURN_AMOUNT;

			// Wrap yaw to avoid floating-point errors if we turn too far
			while (camera.cameraYaw >= 2 * M_PI)
				camera.cameraYaw -= 2 * M_PI;
			while (camera.cameraYaw <= -2 * M_PI)
				camera.cameraYaw += 2 * M_PI;

			// Clamp pitch to stop camera flipping upside down
			if (camera.cameraPitch > degreesToRadians(85))
				camera.cameraPitch = degreesToRadians(85);
			if (camera.cameraPitch < -degreesToRadians(85))
				camera.cameraPitch = -degreesToRadians(85);

			camera.view = translationMat(-camera.cameraPos) * rotateYMat(-camera.cameraYaw) * rotateXMat(-camera.cameraPitch);
			}
	};
};

class Drawable
{
public:
	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;
	BlinnPhongVSConstants blinnPhongVSConstants;
	BlinnPhongPSConstants blinnPhongFSConstants;
	Vector3 position;
	Vector3 eulerRotation;
	Vector3 scale; 
	Matrix4 model;

	Drawable()
	{
		position = { 0,0,0 };
		scale = { 1, 1, 1 };
		eulerRotation = { 0,0,0 };

		setModel();
	}

	Drawable(Vector3 position, Vector3 scale, Vector3 eulerAngles)
	{
		this->position = position;
		this->scale = scale;
		this->eulerRotation = eulerAngles;
		setModel();
	}

	void setModel()
	{
		Matrix4 translation = translationMat(position);
		Matrix4 rotation = rotateXMat(eulerRotation.x) * rotateYMat(eulerRotation.y) * rotateZMat(eulerRotation.z);
		Matrix4 scaleM = scaleMat(scale);

		model =  scaleM * rotation * translation;
	}

	void draw(Shader* shader)
	{
		setModel();
		blinnPhongVSConstants.modelView = model * camera.view;
		blinnPhongVSConstants.modelViewProj = model * camera.view * camera.perspectiveMat;
		blinnPhongVSConstants.normalMatrix = Matrix4ToVector3x3(model);

		shader->Bind();
		shader->SetVertexShaderUniformBuffer("pbVertexConstants", blinnPhongVSConstants);
		vertexBuffer->Bind();
		indexBuffer->Bind();

		d3d11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		d3d11DeviceContext->DrawIndexed(indexBuffer->NumIndices(), 0, 0);
	}
};

class DirecitonalLight
{
	BlinnPhongPSConstants psConstants;
public:
	Vector4 dir;
	Vector4 color;

	void updateDirecitonalLight(Shader* shader)
	{
		psConstants.dirLight.dirEye = dir;
		psConstants.dirLight.color = color;
		shader->SetPixelShaderUniformBuffer("pbPixelConstants", psConstants);
	}
};
class DemoScene : public Scene
{
private:

public:

	void init()
	{
		Scene::init();
	}

	void update(float dt)
	{


		Scene::moveCamera(dt);
		Scene::updateLights(blinnPhongShader, {1.0, 0,0,0}, {1,0,0,1});

		Matrix4 viewMat = translationMat(-camera.cameraPos) * rotateYMat(-camera.cameraYaw) * rotateXMat(-camera.cameraPitch);
		Matrix4 inverseViewMat = inverse(viewMat);
		//Matrix4 inverseViewMat = rotateXMat(camera.cameraPitch) * rotateYMat(camera.cameraYaw) * translationMat(camera.cameraPos);
		// Update the forward vector we use for camera movement:
		camera.cameraFwd = { -viewMat.m[2][0], -viewMat.m[2][1], -viewMat.m[2][2] };
		camera.view = viewMat;



		// Draw lights
		{
			lightShader->Bind();
		}
		// Draw cubes
		{
			blinnPhongShader->Bind();
			Drawable drawable;
			drawable.position = { 3, 0, 0 };
			drawable.scale = { 2, 2, 2 };
			drawable.eulerRotation = { 0, (float)currentTimeInSeconds , 0.0f };
			drawable.setModel();
			drawable.vertexBuffer = sphereVertexBuffer;
			drawable.indexBuffer = sphereIndexBuffer;

			drawable.vertexBuffer->Bind();
			drawable.indexBuffer->Bind();

			drawable.draw(blinnPhongShader);

		}
	}
};