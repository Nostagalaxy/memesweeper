#pragma once
#include "Graphics.h"
#include "RectI.h"

class MemeField
{
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
		void Draw(const Vei2& gridPos, Graphics& gfx) const;
		void Reveal();
		bool IsRevealed() const;
	private:
		bool hasMeme = false;
		State state = State::Hidden;
	};
public:
	MemeField(int nMemes);
	void Draw(Graphics& gfx) const;
private:
	Tile& TileAt(const Vei2& gridPos);
	const Tile& TileAt(const Vei2& gridPos) const;
private:
	RectI& GetRect() const;
	static constexpr int width = 20;
	static constexpr int height = 16;
	Tile field[width * height];
};