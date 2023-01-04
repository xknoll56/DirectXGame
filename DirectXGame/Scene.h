#pragma once
extern Camera camera;
extern double currentTimeInSeconds;
extern ID3D11ShaderResourceView* textureView;
extern ID3D11SamplerState* samplerState;

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


public:
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
			float3 camFwdXZ = normalise(float3{ camera.cameraFwd.x, 0, camera.cameraFwd.z });
			float3 cameraRightXZ = cross(camFwdXZ, { 0, 1, 0 });

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
		}
	};
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

		float4x4 viewMat = translationMat(-camera.cameraPos) * rotateYMat(-camera.cameraYaw) * rotateXMat(-camera.cameraPitch);
		float4x4 inverseViewMat = rotateXMat(camera.cameraPitch) * rotateYMat(camera.cameraYaw) * translationMat(camera.cameraPos);
		// Update the forward vector we use for camera movement:
		camera.cameraFwd = { -viewMat.m[2][0], -viewMat.m[2][1], -viewMat.m[2][2] };



		// Draw lights
		{
			lightShader->Bind();
		}
		// Draw cubes
		{
			blinnPhongShader->Bind();
					// Calculate matrices for cubes
			const int NUM_CUBES = 3;
			float4x4 cubeModelViewMats[NUM_CUBES];
			float3x3 cubeNormalMats[NUM_CUBES];
			{
				float3 cubePositions[NUM_CUBES] = {
					{0.f, 0.f, 0.f},
					{-3.f, 0.f, -1.5f},
					{4.5f, 0.2f, -3.f}
				};

				float modelXRotation = 0.2f * (float)(M_PI * currentTimeInSeconds);
				//float modelXRotation = 0.0f;
				float modelYRotation = 0.1f * (float)(M_PI * currentTimeInSeconds);
				//float modelYRotation = 0.0f;
				for (int i = 0; i < NUM_CUBES; ++i)
				{
					modelXRotation += 0.6f * i; // Add an offset so cubes have different phases
					modelYRotation += 0.6f * i;
					float4x4 modelMat = rotateXMat(modelXRotation) * rotateYMat(modelYRotation) * translationMat(cubePositions[i]);
					float4x4 inverseModelMat = translationMat(-cubePositions[i]) * rotateYMat(-modelYRotation) * rotateXMat(-modelXRotation);
					cubeModelViewMats[i] = modelMat * viewMat;
					float4x4 inverseModelViewMat = inverseViewMat * inverseModelMat;
					cubeNormalMats[i] = float4x4ToFloat3x3(transpose(inverseModelViewMat));
				}
			}
			d3d11DeviceContext->PSSetShaderResources(0, 1, &textureView);
			d3d11DeviceContext->PSSetSamplers(0, 1, &samplerState);
			// Update pixel shader constant buffer
			{
				BlinnPhongPSConstants constants;// = (BlinnPhongPSConstants*)(mappedSubresource.pData);
				constants.dirLight.dirEye = normalise(float4{ 1.f, 1.f, 1.f, 0.f });
				constants.dirLight.color = { 1.0f, 1.0f, 1.0f, 1.f };
				constants.objectColor = { 0.7f, 0.7f, 0.7f , 1.0f };
				constants.useColor = true;
				blinnPhongShader->SetPixelShaderUniformBuffer("pbPixelConstants", constants);
			}

			for (int i = 0; i < NUM_CUBES; ++i)
			{

				BlinnPhongVSConstants constants;// = (BlinnPhongVSConstants*)(mappedSubresource.pData);
				constants.modelViewProj = cubeModelViewMats[i] * camera.perspectiveMat;
				constants.modelView = cubeModelViewMats[i];
				constants.normalMatrix = cubeNormalMats[i];
				blinnPhongShader->SetVertexShaderUniformBuffer("pbVertexConstants", constants);
				cubeVertexBuffer->Bind();
				cubeIndexBuffer->Bind();
				d3d11DeviceContext->DrawIndexed(sphereIndexBuffer->NumIndices(), 0, 0);
			}
		}
	}
};