#pragma once
#include <vector>
#include <string>
#include <array>
#include <memory>

class Graphics;

namespace Bind
{
	class RenderTarget;
	class DepthStencil;
}

namespace Rgph
{
	class Sink;
	class Source;

	class Pass
	{
	public:
		Pass(std::string name) noexcept;
		virtual void Execute(Graphics& gfx) const noexcept(!IS_DEBUG) = 0;
		virtual void Reset() noexcept(!IS_DEBUG);
		const std::string& GetName() const noexcept;
		const std::vector<std::unique_ptr<Sink>>& GetSinks() const;
		Source& GetSource(const std::string& registeredName) const;
		Sink& GetSink(const std::string& registeredName) const;
		void SetSinkLinkage(const std::string& registeredName, const std::string& target);
		virtual void Finalize();
		virtual ~Pass();
	protected:
		void RegisterSink(std::unique_ptr<Sink> sink);
		void RegisterSource(std::unique_ptr<Source> source);
	private:
		std::vector<std::unique_ptr<Sink>> sinks;
		std::vector<std::unique_ptr<Source>> sources;
		std::string name;
	};
}