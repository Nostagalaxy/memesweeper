#include "MemeField.h"
#include <assert.h>
#include <random>
#include "Vei2.h"
#include "SpriteCodex.h"

MemeField::MemeField(int nMemes)
{
	//Check to make sure number of memes parameter is reasonable
	assert(nMemes > 0 && nMemes < width* height);
	std::random_device rd;
	std::mt19937 rng( rd() );
	std::uniform_int_distribution<int> xDist(0, width - 1);
	std::uniform_int_distribution<int> yDist(0, height - 1);

	//Spawn number of memes from nMemes
	for (int nSpawned = 0; nSpawned < nMemes; nSpawned++)
	{
		Vei2 spawnPos; 
		do
		{
			spawnPos = { xDist(rng), yDist(rng) };
		} while (TileAt(spawnPos).HasMeme());

		TileAt(spawnPos).SpawnMeme();
	}
}

void MemeField::Draw(Graphics& gfx) const
{
	//Draw background rect to color in tiles
	gfx.DrawRect(GetRect(), SpriteCodex::baseColor);

	//Loop through field and draw each tile
	for (Vei2 gridPos = { 0,0 }; gridPos.y < height; gridPos.y++)
	{
		for (gridPos.x = 0; gridPos.x < width; gridPos.x++)
		{
			TileAt(gridPos).Draw( gridPos, gfx );
		}
	}
}

void MemeField::OnClickReveal(Vei2 screenPos)
{
	assert(screenPos.x >= 0 && screenPos.x < width * SpriteCodex::tileSize &&
		screenPos.y >= 0 && screenPos.y < height * SpriteCodex::tileSize);

	Vei2 gridPos = ScreenToGrid(screenPos);

	if(!TileAt(gridPos).IsRevealed())
		TileAt(gridPos).Reveal();
}

MemeField::Tile& MemeField::TileAt(const Vei2& gridPos)
{
	return field[gridPos.y * width + gridPos.x];
}

const MemeField::Tile& MemeField::TileAt(const Vei2& gridPos) const
{
	return field[gridPos.y * width + gridPos.x];
}

Vei2& MemeField::ScreenToGrid(Vei2& screenPos) const
{
	return screenPos / SpriteCodex::tileSize;
}

RectI& MemeField::GetRect() const
{
	RectI rect = { 0, width * SpriteCodex::tileSize, 0, height * SpriteCodex::tileSize};
	return rect;
}

bool MemeField::Tile::HasMeme() const
{
	return hasMeme;
}

void MemeField::Tile::SpawnMeme()
{
	assert(!hasMeme);
	hasMeme = true;
}

void MemeField::Tile::Draw(const Vei2& gridPos, Graphics& gfx) const
{
	Vei2 screenPos = gridPos * SpriteCodex::tileSize;

	switch (state)
	{
		case State::Hidden:
		{
			SpriteCodex::DrawTileButton(screenPos, gfx);
			break;
		}
		case State::Flagged:
		{
			SpriteCodex::DrawTileFlag(screenPos, gfx);
			break;
		}
		case State::Revealed:
		{
			//Check if tile has meme
			if (hasMeme)
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
				break;
			}
			else
			{
				SpriteCodex::DrawTile0(screenPos, gfx);
				break;
			}
		}
	}
}

void MemeField::Tile::Reveal()
{
	assert(!IsRevealed());

	state = State::Revealed;
}

bool MemeField::Tile::IsRevealed() const
{
	if (state == State::Revealed)
		return true;
	else
		return false;
}
