# frozen_string_literal: true

require "bundler/gem_tasks"
require "rspec/core/rake_task"

RSpec::Core::RakeTask.new(:spec)

require "rake/extensiontask"

task build: :compile

GEMSPEC = Gem::Specification.load("awscrt.gemspec")

Rake::ExtensionTask.new("awscrt", GEMSPEC) do |ext|
  ext.lib_dir = "lib/awscrt"
end

task default: %i[clobber compile spec]
