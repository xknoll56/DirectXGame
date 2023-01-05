#pragma once
extern Camera camera;
extern double currentTimeInSeconds;
extern ID3D11ShaderResourceView* textureView;
extern ID3D11SamplerState* samplerState;
extern double currentTimeInSeconds;

enum DrawableType
{
	TRIANGLE_LIST = 0,
	LINE_LIST = 1,
	POINT_LIST = 2
};
class Drawable
{
public:
	VertexBuffer* vertexBuffer;
	IndexBuffer* indexBuffer;
	BlinnPhongVSConstants blinnPhongVSConstants;
	Vector3 position;
	Vector3 eulerRotation;
	Vector3 scale;
	Vector4 color;
	Matrix4 model;
	DrawableType topologyType;

	Drawable* childDrawable = nullptr;

	Drawable()
	{
		position = { 0,0,0 };
		scale = { 1, 1, 1 };
		eulerRotation = { 0,0,0 };
		color = { 1,1,1, 1 };
		setModel();
		topologyType = DrawableType::TRIANGLE_LIST;
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
		Matrix4 translation = MatrixTranslate(position);
		Matrix4 rotation = MatrixXRotation(eulerRotation.x) * MatrixYRotation(eulerRotation.y) * MatrixZRotation(eulerRotation.z);
		Matrix4 scaleM = MatrixScale(scale);

		model = scaleM * rotation * translation;
	}
	void draw(Shader* shader, Vector3 position, Vector3 scale, Vector3 eulerRotation)
	{
		this->position = position;
		this->scale = scale;
		this->eulerRotation = eulerRotation;
		setModel();
		//draw(shader);
	}
	void draw(Shader* shader, Matrix4 parentModel)
	{
		setModel();
		model =   model * parentModel;
		blinnPhongVSConstants.modelView = model * camera.view;
		blinnPhongVSConstants.modelViewProj = model * camera.view * camera.perspectiveMat;
		blinnPhongVSConstants.normalMatrix = model.ToMatrix3();
		blinnPhongVSConstants.color = color;

		shader->Bind();
		shader->SetVertexShaderUniformBuffer("pbVertexConstants", blinnPhongVSConstants);

		switch (topologyType)
		{
		case DrawableType::TRIANGLE_LIST:
			d3d11DeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			break;
		case DrawableType::LINE_LIST:
			d3d11DeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
			break;
		case DrawableType::POINT_LIST:
			d3d11DeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);
			break;
		}
		vertexBuffer->Bind();

		if (indexBuffer != nullptr)
		{
			indexBuffer->Bind();
			d3d11DeviceContext->DrawIndexed(indexBuffer->NumIndices(), 0, 0);
		}
		else
		{
			d3d11DeviceContext->Draw(vertexBuffer->NumVerts(), 0);
		}
		if (childDrawable != nullptr)
			childDrawable->draw(shader, model);

	}
};

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
	IndexBuffer* sphereIndexBuffer;

	VertexBuffer* boundingBoxVertexBuffer;

	VertexBuffer* boundingPlaneVertexBuffer;

	Shader* lightShader;
	Shader* blinnPhongShader;


public:

	virtual void updateLights(Shader* shader, Vector4 dirLight, Vector4 color)
	{
		BlinnPhongPSConstants blinnPhongFSConstants;
		blinnPhongFSConstants.dirLight.dirEye = VectorNormalize(Vector4{ 1.f, 1.f, 1.f, 0.f });
		blinnPhongFSConstants.dirLight.color = { 1.0f, 1.0f, 1.0f, 1.f };
		blinnPhongShader->SetPixelShaderUniformBuffer("pbPixelConstants", blinnPhongFSConstants);
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

		boundingBoxVertexBuffer = new VertexBuffer(boundingBoxVerts, 24, 3 * sizeof(float), 0, false);

		boundingPlaneVertexBuffer = new VertexBuffer(boundingPlaneVerts, 8, 3 * sizeof(float), 0, false);



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

	void drawBox(Vector3 position, Vector3 scale, Vector3 eulerAngles, Vector4 color)
	{
		Matrix4 translation = MatrixTranslate(position);
		Matrix4 rotation = MatrixXRotation(eulerAngles.x) * MatrixYRotation(eulerAngles.y) * MatrixZRotation(eulerAngles.z);
		Matrix4 scaleM = MatrixScale(scale);

		Matrix4 model = scaleM * rotation * translation;
		BlinnPhongVSConstants blinnPhongVSConstants;
		blinnPhongVSConstants.modelView = model * camera.view;
		blinnPhongVSConstants.modelViewProj = model * camera.view * camera.perspectiveMat;
		blinnPhongVSConstants.normalMatrix = model.ToMatrix3();
		blinnPhongVSConstants.color = color;

		blinnPhongShader->Bind();
		blinnPhongShader->SetVertexShaderUniformBuffer("pbVertexConstants", blinnPhongVSConstants);
		d3d11DeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cubeVertexBuffer->Bind();
		cubeIndexBuffer->Bind();
		d3d11DeviceContext->DrawIndexed(cubeIndexBuffer->NumIndices(), 0, 0);

	}

	void drawSphere(Vector3 position, Vector3 scale, Vector3 eulerAngles, Vector4 color)
	{
		Matrix4 translation = MatrixTranslate(position);
		Matrix4 rotation = MatrixXRotation(eulerAngles.x) * MatrixYRotation(eulerAngles.y) * MatrixZRotation(eulerAngles.z);
		Matrix4 scaleM = MatrixScale(scale);

		Matrix4 model = scaleM * rotation * translation;
		BlinnPhongVSConstants blinnPhongVSConstants;
		blinnPhongVSConstants.modelView = model * camera.view;
		blinnPhongVSConstants.modelViewProj = model * camera.view * camera.perspectiveMat;
		blinnPhongVSConstants.normalMatrix = model.ToMatrix3();
		blinnPhongVSConstants.color = color;

		blinnPhongShader->Bind();
		blinnPhongShader->SetVertexShaderUniformBuffer("pbVertexConstants", blinnPhongVSConstants);
		d3d11DeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		sphereVertexBuffer->Bind();
		sphereIndexBuffer->Bind();
		d3d11DeviceContext->DrawIndexed(sphereIndexBuffer->NumIndices(), 0, 0);

	}

	void drawPlane(Vector3 position, Vector3 scale, Vector3 eulerAngles, Vector4 color)
	{
		Matrix4 translation = MatrixTranslate(position);
		Matrix4 rotation = MatrixXRotation(eulerAngles.x) * MatrixYRotation(eulerAngles.y) * MatrixZRotation(eulerAngles.z);
		Matrix4 scaleM = MatrixScale(scale);

		Matrix4 model = scaleM * rotation * translation;
		BlinnPhongVSConstants blinnPhongVSConstants;
		blinnPhongVSConstants.modelView = model * camera.view;
		blinnPhongVSConstants.modelViewProj = model * camera.view * camera.perspectiveMat;
		blinnPhongVSConstants.normalMatrix = model.ToMatrix3();
		blinnPhongVSConstants.color = color;

		blinnPhongShader->Bind();
		blinnPhongShader->SetVertexShaderUniformBuffer("pbVertexConstants", blinnPhongVSConstants);
		d3d11DeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		planeVertexBuffer->Bind();
		planeIndexBuffer->Bind();
		d3d11DeviceContext->DrawIndexed(planeIndexBuffer->NumIndices(), 0, 0);

	}

	void drawBoundingBox(Vector3 position, Vector3 scale, Vector3 eulerAngles, Vector4 color)
	{
		Matrix4 translation = MatrixTranslate(position);
		Matrix4 rotation = MatrixXRotation(eulerAngles.x) * MatrixYRotation(eulerAngles.y) * MatrixZRotation(eulerAngles.z);
		Matrix4 scaleM = MatrixScale(scale);

		Matrix4 model = scaleM * rotation * translation;
		BlinnPhongVSConstants blinnPhongVSConstants;
		blinnPhongVSConstants.modelView = model * camera.view;
		blinnPhongVSConstants.modelViewProj = model * camera.view * camera.perspectiveMat;
		blinnPhongVSConstants.normalMatrix = model.ToMatrix3();
		blinnPhongVSConstants.color = color;

		blinnPhongShader->Bind();
		blinnPhongShader->SetVertexShaderUniformBuffer("pbVertexConstants", blinnPhongVSConstants);
		d3d11DeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
		boundingBoxVertexBuffer->Bind();
		d3d11DeviceContext->Draw(boundingBoxVertexBuffer->NumVerts(), 0);

	}

	void drawBoundingPlane(Vector3 position, Vector3 scale, Vector3 eulerAngles, Vector4 color)
	{
		Matrix4 translation = MatrixTranslate(position);
		Matrix4 rotation = MatrixXRotation(eulerAngles.x) * MatrixYRotation(eulerAngles.y) * MatrixZRotation(eulerAngles.z);
		Matrix4 scaleM = MatrixScale(scale);

		Matrix4 model = scaleM * rotation * translation;
		BlinnPhongVSConstants blinnPhongVSConstants;
		blinnPhongVSConstants.modelView = model * camera.view;
		blinnPhongVSConstants.modelViewProj = model * camera.view * camera.perspectiveMat;
		blinnPhongVSConstants.normalMatrix = model.ToMatrix3();
		blinnPhongVSConstants.color = color;

		blinnPhongShader->Bind();
		blinnPhongShader->SetVertexShaderUniformBuffer("pbVertexConstants", blinnPhongVSConstants);
		d3d11DeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
		boundingPlaneVertexBuffer->Bind();
		d3d11DeviceContext->Draw(boundingPlaneVertexBuffer->NumVerts(), 0);

	}

	void moveCamera(float dt)
	{
		// Update camera
		{
			Vector3 camFwdXZ = VectorNormalize(Vector3{ camera.cameraFwd.x, 0, camera.cameraFwd.z });
			Vector3 cameraRightXZ = VectorCross(camFwdXZ, { 0, 1, 0 });

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

			camera.view = MatrixYRotation(-camera.cameraYaw) * MatrixXRotation(-camera.cameraPitch);
			camera.cameraFwd = { -camera.view.mat[2][0], -camera.view.mat[2][1], -camera.view.mat[2][2] };
			camera.view = MatrixTranslate(-camera.cameraPos) * camera.view;
		}
	};
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
	Drawable drawable;
	Drawable childDrawable;
	VertexBuffer* boundingBox;

public:

	void init()
	{
		Scene::init();
		drawable.position = { 3, 0, 3 };
		drawable.scale = { 2, 2, 2 };
		drawable.color = { 1,1,0,1 };
		drawable.eulerRotation = { (float)currentTimeInSeconds, (float)currentTimeInSeconds , (float)currentTimeInSeconds };
		drawable.setModel();
		drawable.vertexBuffer = cubeVertexBuffer;
		drawable.indexBuffer = cubeIndexBuffer;
		drawable.topologyType = DrawableType::TRIANGLE_LIST; 

		childDrawable.position = { 0, 3, 0 };
		childDrawable.scale = { 1, 1, 1 };
		childDrawable.color = { 1,1,0,1 };
		childDrawable.eulerRotation = { 0,0,0 };
		childDrawable.setModel();
		childDrawable.vertexBuffer = sphereVertexBuffer;
		childDrawable.indexBuffer = sphereIndexBuffer;
		childDrawable.topologyType = DrawableType::TRIANGLE_LIST;

		drawable.childDrawable = &childDrawable;


			camera.cameraPos += {0, 1, 0};

	}

	void update(float dt)
	{


		Scene::moveCamera(dt);
		Scene::updateLights(blinnPhongShader, { 1.0, 0,0,0 }, { 1,0,0,1 });

		//drawBoundingBox({ 2,1,0 }, { 1,1,1 }, { (float)currentTimeInSeconds,(float)currentTimeInSeconds,(float)currentTimeInSeconds }, { 0, 0, 1, 0 });
		//drawBox({ 2,1,0 }, { 1,1,1 }, { (float)currentTimeInSeconds,(float)currentTimeInSeconds,(float)currentTimeInSeconds }, { 1, 1, 1, 0 });
		//drawSphere({ -2,1,0 }, { 1,1,1 }, { (float)currentTimeInSeconds,(float)currentTimeInSeconds,(float)currentTimeInSeconds }, { 1, 1, 1, 0 });
		drawPlane({ 0,0,0 }, { 10,1,10 }, { 0,0,0 }, { 1, 1, 1, 0 });
		//drawBoundingPlane({ 0,0,0 }, { 9.9,1,9.9 }, { 0,0,0 }, { 1, 0, 0, 0 });
		//drawable.draw(blinnPhongShader);
		drawable.eulerRotation = { (float)currentTimeInSeconds, (float)currentTimeInSeconds , (float)currentTimeInSeconds };
		Matrix4 mat(1.0f);
		drawable.draw(blinnPhongShader, mat);

	}
};