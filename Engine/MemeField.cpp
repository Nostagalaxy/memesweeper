#include "MemeField.h"
#include <assert.h>
#include <random>
#include "Vei2.h"
#include "SpriteCodex.h"


MemeField::MemeField(int nMemes)
	:
	gameOverSfx(L"Engine_spayed.wav"),
	nonMemeTilesLeft((height * width) - nMemes)
{
	//Set location in center of screen
	Vei2 center = {Graphics::ScreenWidth / 2 , Graphics::ScreenHeight / 2};
	Vei2 toStartLoc = { (width * SpriteCodex::tileSize) / 2 , (height * SpriteCodex::tileSize) / 2};
	location = center - toStartLoc;

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
			//Create tile location in screen space and pass to tile draw
			Vei2 screenPos = GridToScreen(gridPos);
			TileAt(gridPos).Draw( screenPos, gameState, gfx );
		}
	}

	if (gameState == State::Winrar)
	{
		SpriteCodex::DrawWin(location, gfx);
	}
}

void MemeField::OnClickReveal(Vei2 screenPos)
{
	RectI gridRect = GetRect();

	assert(screenPos.x >= gridRect.left && screenPos.x < gridRect.right &&
		screenPos.y >= gridRect.top && screenPos.y < gridRect.bottom);

	Vei2 gridPos = ScreenToGrid(screenPos);
	
	RevealTile(gridPos);
	
	if (nonMemeTilesLeft <= 0)
	{
		gameState = State::Winrar;
	}
}

void MemeField::OnClickFlag(Vei2 screenPos)
{
	RectI gridRect = GetRect();

	assert(screenPos.x >= gridRect.left && screenPos.x < gridRect.right &&
		screenPos.y >= gridRect.top && screenPos.y < gridRect.bottom);

	Vei2 gridPos = ScreenToGrid(screenPos);

	if (!TileAt(gridPos).IsRevealed())
	{
		TileAt(gridPos).ToggleFlag();
	}
}

void MemeField::RevealTile(const Vei2& gridPos)
{
	Tile& tile = TileAt(gridPos);
	//Only reveal if the tile is not flagged and note revealed already
	if (!tile.IsRevealed() && !tile.IsFlagged())
	{
		tile.Reveal();

		if (tile.HasMeme())
		{
			gameState = State::Fucked;
			gameOverLoc = gridPos;
			gameOverSfx.Play();
		}
		else if (!tile.HasMeme())
		{
			nonMemeTilesLeft--;
		}
		if (tile.HasNoNeighborMemes())
		{
			const int xStart = std::max(gridPos.x - 1, 0);
			const int yStart = std::max(gridPos.y - 1, 0);
			const int xEnd = std::min(gridPos.x + 1, width - 1);
			const int yEnd = std::min(gridPos.y + 1, height - 1);

			for (Vei2 gridPos = { xStart, yStart }; gridPos.y <= yEnd; gridPos.y++)
			{
				for (gridPos.x = xStart; gridPos.x <= xEnd; gridPos.x++)
				{	
					RevealTile(gridPos);
				}
			}
		}
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
	return (screenPos - location) / SpriteCodex::tileSize;
}

Vei2& MemeField::GridToScreen(Vei2& gridPos) const
{
	return (gridPos * SpriteCodex::tileSize) + location;
}

RectI& MemeField::GetRect() const
{
	RectI rect = { location.x, (width * SpriteCodex::tileSize) + location.x, location.y, (height * SpriteCodex::tileSize) + location.y};
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

bool MemeField::Fucked() const
{
	if (gameState == State::Fucked)
		return true;
	else
		return false;
}

bool MemeField::GameWon() const
{
	if (gameState == State::Winrar)
		return true;
	else
		return false;
}

MemeField::State MemeField::getState() const
{
	return gameState;
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

void MemeField::Tile::Draw(const Vei2& screenPos, const MemeField::State gameState, Graphics& gfx) const
{
	if (gameState == MemeField::State::Play || gameState == MemeField::State::Winrar)
	{
		switch (state)
		{
		case Tile::State::Hidden:
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
	else if (gameState == MemeField::State::Fucked)
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
				SpriteCodex::DrawTile0(screenPos, gfx);
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

bool MemeField::Tile::HasNoNeighborMemes() const
{
	return nNeighborMemes == 0;
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
