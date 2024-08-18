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

	//Count nMemes for each tile
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			const Vei2 gridPos(x, y);

			TileAt(gridPos).SetNeighborMemeCount(CountNeighborMemes(gridPos));
		}
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
			TileAt(gridPos).Draw( gridPos, isFucked, gfx );
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
	if (TileAt(gridPos).HasMeme())
	{
		isFucked = true;
		gameOverLoc = gridPos;
	}
}

void MemeField::OnClickFlag(Vei2 screenPos)
{
	assert(screenPos.x >= 0 && screenPos.x < width * SpriteCodex::tileSize &&
		screenPos.y >= 0 && screenPos.y < height * SpriteCodex::tileSize);

	Vei2 gridPos = ScreenToGrid(screenPos);

	if (!TileAt(gridPos).IsRevealed())
	{
		TileAt(gridPos).ToggleFlag();
	}
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

int MemeField::CountNeighborMemes(const Vei2& gridPos)
{
	//Set boundaries of start and stop position
	//Start is x-1, y-1 and end is x + 1, y + 1, unless tile is near the edge of grid (use std::min and std::max)
	const int xStart = std::max(gridPos.x - 1, 0);
	const int yStart = std::max(gridPos.y - 1, 0);
	const int xEnd = std::min(gridPos.x + 1, width - 1);
	const int yEnd = std::min(gridPos.y + 1, height - 1);

	int memeCount = 0;

	//loop
	for (int y = yStart; y <= yEnd; y++)
	{
		for (int x = xStart; x <= xEnd; x++)
		{
			const Vei2 gridPos(x, y);
			if (TileAt(gridPos).HasMeme())
			{
				memeCount++;
			}
		}
	}
	return memeCount;
}

bool MemeField::Fucked()
{
	return isFucked;
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

void MemeField::Tile::SetNeighborMemeCount(const int memeCount)
{
	//assert variable is uninitialized
	assert(nNeighborMemes == -1);

	nNeighborMemes = memeCount;
}

void MemeField::Tile::Draw(const Vei2& gridPos, bool gameOver, Graphics& gfx) const
{
	Vei2 screenPos = gridPos * SpriteCodex::tileSize;

	if (!gameOver)
	{
		switch (state)
		{
		case State::Hidden:
		{
			SpriteCodex::DrawTileButton(screenPos, gfx);
			break;
		}
		case State::Flagged:
		{
			SpriteCodex::DrawTileButton(screenPos, gfx);
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
				SpriteCodex::DrawNumberTile(screenPos, nNeighborMemes, gfx);
				break;
			}
		}
		}
	}
	else if (gameOver)
	{
		switch (state)
		{
		case State::Hidden:
		{
			if (hasMeme)
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
				break;
			}
			else
			{
				SpriteCodex::DrawTileButton(screenPos, gfx);
				break;
			}
		}
		case State::Flagged:
		{
			if (!hasMeme)
			{
				SpriteCodex::DrawTileCross(screenPos, gfx);
				break;
			}
			else if(hasMeme)
			{
				SpriteCodex::DrawTileBomb(screenPos, gfx);
				SpriteCodex::DrawTileFlag(screenPos, gfx);
				break;
			}
		}
		case State::Revealed:
		{
			//Check if tile has meme
			if (hasMeme)
			{
				if (isGameOverLoc)
				{
					SpriteCodex::DrawTileBombRed(screenPos, gfx);
				}
				SpriteCodex::DrawTileBomb(screenPos, gfx);
				break;
			}
			else
			{
				SpriteCodex::DrawNumberTile(screenPos, nNeighborMemes, gfx);
				break;
			}
		}
		}
	}
}

void MemeField::Tile::Reveal()
{
	assert(!IsRevealed());

	state = State::Revealed;

	if (hasMeme)
	{
		isGameOverLoc = true;
	}
}

bool MemeField::Tile::IsRevealed() const
{
	return (state == State::Revealed);
}

void MemeField::Tile::ToggleFlag()
{
	assert(!IsRevealed());

	if (state == State::Hidden)
	{
		state = State::Flagged;
	}
	else
	{
		state = State::Hidden;
	}
}

bool MemeField::Tile::IsFlagged()
{
	return (state == State::Flagged);
}
