#pragma once
#include "Graphics.h"
#include "RectI.h"
#include "Sound.h"

class MemeField
{
	enum class State
	{
		Fucked,
		Winrar,
		Play
	};

	class Tile
	{
	public:
		enum class State
		{
			Hidden,
			Flagged,
			Revealed
		};
	public:
		bool HasMeme() const;
		void SpawnMeme();
		void SetNeighborMemeCount(const int memeCount); //Set the number of neighboring memes for the tile.
		void Draw(const Vei2& screenPos, const MemeField::State gameState, Graphics& gfx) const;
		bool HasNoNeighborMemes() const;
		void Reveal();
		bool IsRevealed() const;
		void ToggleFlag();
		bool IsFlagged();
	private:
		bool isGameOverLoc = false;
		int nNeighborMemes = -1;
		bool hasMeme = false;
		State state = State::Hidden;
	};
public:
	MemeField(const int nMemes);
	void Draw(Graphics& gfx) const;
	void OnClickReveal(Vei2 screenPos);
	void OnClickFlag(Vei2 screenPos);
	RectI& GetRect() const;
	bool Fucked() const;
	bool GameWon() const;
	State getState() const;
private:
	int CountNeighborMemes(const Vei2& gridPos);
	void RevealTile(const Vei2& gridPos);
	Tile& TileAt(const Vei2& gridPos);
	const Tile& TileAt(const Vei2& gridPos) const;
	Vei2& ScreenToGrid(Vei2& screenPos) const;
	Vei2& GridToScreen(Vei2& gridPos) const;
private:
	State gameState = State::Play;
	Sound gameOverSfx;
	Vei2 location;
	Vei2 gameOverLoc = { -1, -1 };
	int nonMemeTilesLeft;
	static constexpr int width = 20;
	static constexpr int height = 16;
	Tile field[width * height];
};