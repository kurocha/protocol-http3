# Teapot v3.5.0 configuration generated at 2026-05-24 00:00:00 +0900

required_version "3.0"

define_project "protocol-http3" do |project|
	project.title = "Protocol HTTP3"
end

# Build Targets

define_target 'protocol-http3-library' do |target|
	target.depends 'Language/C++17'
	target.depends 'Build/Compile/Commands'
	
	target.depends 'Library/Protocol/QUIC', public: true
	target.depends 'Library/nghttp3', public: true
	
	target.provides 'Library/Protocol/HTTP3' do
		source_root = target.package.path + 'source'
		
		library_path = build static_library: 'ProtocolHTTP3', source_files: source_root.glob('Protocol/HTTP3/**/*.cpp')
		
		append linkflags library_path
		append header_search_paths source_root
		
		compile_commands destination_path: (source_root + "compile_commands.json")
	end
end

define_target 'protocol-http3-test' do |target|
	target.depends 'Library/Protocol/HTTP3'
	target.depends 'Library/UnitTest'
	
	target.depends 'Language/C++17'
	target.depends 'Build/Compile/Commands'
	
	target.provides 'Test/Protocol/HTTP3' do |arguments|
		test_root = target.package.path + 'test'
		
		run tests: 'ProtocolHTTP3-tests', source_files: test_root.glob('Protocol/HTTP3/**/*.cpp'), arguments: arguments
		
		compile_commands destination_path: (test_root + "compile_commands.json")
	end
end

# Configurations

define_configuration 'development' do |configuration|
	configuration[:source] = "https://github.com/kurocha"
	configuration.import "protocol-http3"
	
	# Provides all the build related infrastructure:
	configuration.require 'platforms'
	
	# Provides unit testing infrastructure and generators:
	configuration.require 'unit-test'
	
	# Provides some useful C++ generators:
	configuration.require 'generate-template'
	configuration.require 'generate-cpp-class'
	
	configuration.require "build-compile-commands"
end

define_configuration "protocol-http3" do |configuration|
	configuration.public!
	
	configuration.require "protocol-quic"
	configuration.require "scheduler"
	configuration.require "nghttp3"
end
