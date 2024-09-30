#include "raylib/raylib.h"
#include "box2d/box2d.h"

#include <assert.h>

// This shows how to use Box2D v3 with raylib.
// It also show how to use Box2D with pixel units.

typedef struct Entity
{
	b2BodyId bodyId;
	b2Vec2 extent;
	Texture texture;
} Entity;

void DrawEntity(const Entity* entity)
{
	// The boxes were created centered on the bodies, but raylib draws textures starting at the top left corner.
	// b2Body_GetWorldPoint gets the top left corner of the box accounting for rotation.
	b2Vec2 p = b2Body_GetWorldPoint(entity->bodyId, (b2Vec2) { -entity->extent.x, -entity->extent.y });
	b2Rot rotation = b2Body_GetRotation(entity->bodyId);
	float radians = b2Rot_GetAngle(rotation);

	Vector2 ps = {p.x, p.y};
	DrawTextureEx(entity->texture, ps, RAD2DEG * radians, 1.0f, WHITE);
    //DrawText( TextFormat( "id: %d", entity->bodyId ), ps.x, ps.y, 20, GREEN );

	// I used these circles to ensure the coordinates are correct
	//DrawCircleV(ps, 5.0f, BLACK);
	//p = b2Body_GetWorldPoint(entity->bodyId, (b2Vec2){0.0f, 0.0f});
	//ps = (Vector2){ p.x, p.y };
	//DrawCircleV(ps, 5.0f, BLUE);
	//p = b2Body_GetWorldPoint(entity->bodyId, (b2Vec2){ entity->extent.x, entity->extent.y });
	//ps = (Vector2){ p.x, p.y };
	//DrawCircleV(ps, 5.0f, RED);
}

#define GROUND_COUNT 24
#define BOX_COUNT 55

int main(void)
{
	int width = 800, height = 450;
	InitWindow(width, height, "Exemplo Box2D");

	SetTargetFPS(60);

	// 32 pixels per meter is a appropriate for this scene. The boxes are 32 pixels wide.
	float lengthUnitsPerMeter = 32.0f;
	b2SetLengthUnitsPerMeter(lengthUnitsPerMeter);

	b2WorldDef worldDef = b2DefaultWorldDef();

	// Realistic gravity is achieved by multiplying gravity by the length unit.
	worldDef.gravity.y = 9.8f * lengthUnitsPerMeter;
	b2WorldId worldId = b2CreateWorld(&worldDef);

	Texture groundTexture = LoadTexture("resources/images/ground32.png");
	Texture boxTexture = LoadTexture("resources/images/box32.png");

	b2Vec2 groundExtent = { 0.5f * groundTexture.width, 0.5f * groundTexture.height };
	b2Vec2 boxExtent = { 0.5f * boxTexture.width, 0.5f * boxTexture.height };

	// These polygons are centered on the origin and when they are added to a body they
	// will be centered on the body position.
	b2Polygon groundPolygon = b2MakeBox(groundExtent.x, groundExtent.y);
	b2Polygon boxPolygon = b2MakeBox(boxExtent.x, boxExtent.y);

	Entity groundEntities[GROUND_COUNT] = { 0 };
	for (int i = 0; i < GROUND_COUNT; ++i)
	{
		Entity* entity = groundEntities + i;
		b2BodyDef bodyDef = b2DefaultBodyDef();
		bodyDef.position = (b2Vec2){ (2.0f * i + 2.0f) * groundExtent.x, height - groundExtent.y - 100.0f};

		// I used this rotation to test the world to screen transformation
		//bodyDef.rotation = b2MakeRot(0.25f * b2_pi * i);

		entity->bodyId = b2CreateBody(worldId, &bodyDef);
		entity->extent = groundExtent;
		entity->texture = groundTexture;
		b2ShapeDef shapeDef = b2DefaultShapeDef();
		b2CreatePolygonShape(entity->bodyId, &shapeDef, &groundPolygon);
	}

	Entity boxEntities[BOX_COUNT] = { 0 };
	int boxIndex = 0;
	for (int i = 0; i < 10; ++i)
	{
		float y = height - groundExtent.y - 100.0f - (2.5f * i + 2.0f) * boxExtent.y - 20.0f;

		for (int j = i; j < 10; ++j)
		{
			float x = 0.5f * width + (3.0f * j - i - 3.0f) * boxExtent.x;
			assert(boxIndex < BOX_COUNT);

			Entity* entity = boxEntities + boxIndex;
			b2BodyDef bodyDef = b2DefaultBodyDef();
			bodyDef.type = b2_dynamicBody;
			bodyDef.position = (b2Vec2){ x, y };
            //bodyDef.linearVelocity = (b2Vec2){ -200.0f, 0.0f };
			entity->bodyId = b2CreateBody(worldId, &bodyDef);
			entity->texture = boxTexture;
			entity->extent = boxExtent;
			b2ShapeDef shapeDef = b2DefaultShapeDef();
			b2CreatePolygonShape(entity->bodyId, &shapeDef, &boxPolygon);

			boxIndex += 1;
		}
	}

	bool pause = false;

	while (!WindowShouldClose())
	{
		if (IsKeyPressed(KEY_P))
		{
			pause = !pause;
		}

        if ( IsKeyPressed( KEY_SPACE ) ) {
            Entity* entity = boxEntities + 54;
            //TraceLog( LOG_INFO, "%d", entity->bodyId );
            b2Vec2 center = b2Body_GetWorldPoint(entity->bodyId, (b2Vec2) { entity->extent.x, entity->extent.y });
            b2Body_ApplyLinearImpulse( entity->bodyId, (b2Vec2){ 0.0f, -200000.0f }, center, false );
            //b2Body_ApplyLinearImpulseToCenter( entity->bodyId, (b2Vec2){ 0.0f, -200.0f }, false );
            //b2Body_ApplyAngularImpulse( entity->bodyId, 1000.0f, true );
            //b2Body_ApplyForce( entity->bodyId, (b2Vec2){ 0.0f, -1.0f }, center, true );
        }

		if (pause == false)
		{
			float deltaTime = GetFrameTime();
			b2World_Step(worldId, deltaTime, 4);
		}

		BeginDrawing();
		ClearBackground(DARKGRAY);

		for (int i = 0; i < GROUND_COUNT; ++i)
		{
			DrawEntity(groundEntities + i);
		}

		for (int i = 0; i < BOX_COUNT; ++i)
		{
			DrawEntity(boxEntities + i);
		}

        DrawFPS( 20, 20 );

		EndDrawing();
	}

	UnloadTexture(groundTexture);
	UnloadTexture(boxTexture);

	CloseWindow();

	return 0;
}